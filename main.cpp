#include "header.h"

int main(int argc, char* argv[])
{
	string data_path = "C:\\Users\\jpollack\\Documents\\github\\ogl_testing\\";
	//string data_path = "J:\\Github\\ogl_testing\\";
	string text_file = data_path + "text_template.bmp";
	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	float eye_level = 1.65f; //7.65 for car
	shared_ptr<ogl_context> context(new ogl_context("OpenGL Shader Test", vert_file.c_str(), frag_file.c_str(), 1280, 720));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<texture_handler> textures(new texture_handler(data_path));
	shared_ptr<ogl_camera_free> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 1.0f), 45.0f));

	vector<mesh_data> model_meshes = jep::generateMeshes((data_path + "core_logo_facenormals.obj").c_str());
	map<string, boost::shared_ptr<material_data> > model_materials = jep::generateMaterials((data_path + "core_logo_facenormals.mtl").c_str(), textures, context);

	vector< shared_ptr<jep::ogl_data> > environment_mesh_data;

	glCullFace(GL_BACK);

	textures->addTextureByFilename("core_normals", "core_normals.bmp");
	textures->addTextureByFilename("transparency", "transparency.bmp");
	textures->addTextureByFilename("brick_normals", "brick_normals.bmp");
	textures->addTextureByFilename("specular", "specular.bmp");
	textures->addTextureByFilename("vase_specular", "vase_specular.bmp");

	for (const auto &i : model_meshes)
	{
		vector<unsigned short> mesh_indices;

		shared_ptr<jep::ogl_data> env_mesh(new jep::ogl_data(
			context,
			model_materials.at(i.getMaterialName()),
			GL_STATIC_DRAW,
			i.getElementIndex(),
			i.getIndexedVertexData(),
			i.getVSize(),
			i.getVTSize(),
			i.getVNSize()
			));

		environment_mesh_data.push_back(env_mesh);
		cout << i.getMeshlName() << ": " << i.getMaterialName() << endl;
	}
	
	boost::shared_ptr<material_data> core_material = model_materials.at("lambert2SG");
	core_material->setTextureData("normal", "core_normals");

	boost::shared_ptr<material_data> shell_material = model_materials.at("lambert3SG");
	shell_material->setTextureData("normal", "core_normals");
	shell_material->setTextureData("bump", "lambert2SG_bump");

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	clock_t start = clock();

	mat4 projection_matrix;

	float light_rotation_radians = 0.01;
	float current_radians = 0.0;
	mat4 light_rotation_matrix = glm::rotate(mat4(1.0), current_radians, vec3(0.0, 1.0, 0.0));

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			if ((clock() - start) / CLOCKS_PER_SEC > 2.0f)
				start = clock();

			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();

			if (current_radians < 6.28318530718)
				current_radians += light_rotation_radians;

			else current_radians = 0.0;

			//light_rotation_matrix = glm::rotate(mat4(1.0), current_radians, vec3(0.0, 1.0, 0.0));

			glUniformMatrix4fv(context->getShaderGLint("light_transform"), 1, GL_FALSE, &light_rotation_matrix[0][0]);

			for (auto mesh : environment_mesh_data)
			{
				glBindVertexArray(*(mesh->getVAO()));
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				glEnableVertexAttribArray(4);

				mesh->getMaterial()->setShader();
				
				//TODO try removing this line
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(mesh->getIND()));

				camera->setMVP(context, mat4(1.0f), jep::NORMAL);

				//glDrawArrays(GL_TRIANGLES, 0, opengl_data->getVertexCount());
				glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_SHORT, (void*)0);
				glBindTexture(GL_TEXTURE_2D, 0);

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				glDisableVertexAttribArray(3);
				glDisableVertexAttribArray(4);
				glBindVertexArray(0);
			}

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
				finished = true;

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}
}