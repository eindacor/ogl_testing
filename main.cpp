#include "header.h"

int main(int argc, char* argv[])
{
	//string data_path = "C:\\Users\\jpollack\\Documents\\github\\ogl_testing\\";
	string data_path = "J:\\Github\\ogl_testing\\";
	string text_file = data_path + "text_template.bmp";
	string vert_file = data_path + "vertex_shader.glsl";
	string frag_file = data_path + "fragment_shader.glsl";

	float eye_level = 1.65f;
	shared_ptr<ogl_context> context(new ogl_context("OpenGL Shader Test", vert_file.c_str(), frag_file.c_str(), 1280, 720));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<texture_handler> textures(new texture_handler(data_path));
	shared_ptr<ogl_camera_free> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 3.0f), 45.0f));

	vector<mesh_data> model_meshes = jep::generateMeshes((data_path + "vase.obj").c_str());
	map<string, boost::shared_ptr<material_data> > model_materials = jep::generateMaterials((data_path + "vase.mtl").c_str(), textures, context);

	vector< shared_ptr<jep::ogl_data> > environment_mesh_data;

	textures->addTexture("core_normals", data_path + "core_normals.bmp");
	textures->addTexture("transparency", data_path + "transparency.bmp");
	textures->addTexture("brick_normals", data_path + "brick_normals.bmp");
	textures->addTexture("specular", data_path + "specular.bmp");
	textures->addTexture("vase_specular", data_path + "vase_specular.bmp");
	//textures->addTexture("00", data_path + "normal_00.bmp");
	//textures->addTexture("01", data_path + "normal_01.bmp");
	//textures->addTexture("02", data_path + "normal_02.bmp");
	//textures->addTexture("03", data_path + "normal_03.bmp");
	//textures->addTexture("cloth_diffuse", data_path + "cloth_diffuse.bmp");
	//textures->addTexture("cloth_spec", data_path + "cloth_spec.bmp");
	//textures->addTexture("cloth_normal", data_path + "cloth_normal.bmp");
	//textures->addTexture("normal_test", data_path + "normal_test.bmp");

	model_materials.at("lambert2SG")->setTextureData("specular", "vase_specular");
	model_materials.at("lambert2SG")->setSpecularValue(0.5);
	model_materials.at("lambert2SG")->setSpecularDampening(30);
	model_materials.at("lambert3SG")->setTextureData("normal", "wood_normal.bmp");

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
	}

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

			light_rotation_matrix = glm::rotate(mat4(1.0), current_radians, vec3(0.0, 1.0, 0.0));

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