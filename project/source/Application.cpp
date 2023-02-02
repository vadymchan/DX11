#include "Application.h"

void Application::WindowSetup(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
{

#ifdef _DEBUG
	window.initConsole();
#endif
	window.initWindow(title, yStart, xStart, width, height, appHandle, windowShowParams);
	window.initColorBuffer();

}

void Application::Init(const HINSTANCE& appHandle, int windowShowParams)
{


	const int ScreenWidth{ 640 };
	const int ScreenHeight{ 480 };

	cameraAspectRatio = static_cast<float>(ScreenWidth) / ScreenHeight;
	cameraFov = glm::radians(60.0f);
	cameraNearZ = 0.1f;
	cameraFarZ = 1000.0f;
	cameraSpeed = 0.05f;
	rotationSpeed = 0.0005f;
	rollRotationCoeff = 5;
	dragCoef = 1.4;
	glm::vec3 cameraPosition{ glm::vec3(0,0,5) };
	glm::vec3 cameraForward{ glm::vec3(0.0, 0.0f, -1.0f) };
	glm::vec3 worldUp{ glm::vec3(0, 1, 0) };
	nearPlaneHeight = cameraNearZ * glm::tan(cameraFov / 2) * 2 ;
	nearPlaneWidth = nearPlaneHeight * cameraAspectRatio ;


	//planes
	//----------------------------------------------------------------------------------
	Engine::Material planeMaterial = Engine::Material({ 0.135, 0.2225, 0.1575 }, { 0.54, 0.89, 0.63 }, { 0.316228, 0.316228,0.316228 }, 0.1);
	auto mathPlane = Engine::Plane(worldUp, glm::vec3(0, -5, 0));
	std::vector<Engine::ColoredPlane> planes
	{
		Engine::ColoredPlane(planeMaterial, mathPlane)
	};
	//----------------------------------------------------------------------------------


	//spheres
	//----------------------------------------------------------------------------------
	Engine::Material sphereMaterial = Engine::Material({ 0.1745, 0.01175, 0.01175 }, { 0.61424,	0.04136	,0.04136 }, { 0.727811,	0.626959, 0.626959 }, 0.6);
	auto mathSphere = Engine::Sphere(glm::vec3(10, 0, 5), 2); 

	std::vector<Engine::ColoredSphere> spheres
	{
		Engine::ColoredSphere(sphereMaterial, mathSphere),
	};
	//----------------------------------------------------------------------------------


	//triangles - example
	//----------------------------------------------------------------------------------
	Engine::Material triangleMaterial = Engine::Material({ 0,0,0 }, { 0.61424,	0.04136	,0.04136 }, { 0.727811,	0.626959, 0.626959 }, 0.6);
	auto mathTriangle = Engine::Triangle(glm::vec3(-1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, -1.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.0f, 1.0f, 0.0f));
	std::vector<Engine::ColoredTriangle> triangles
	{
		//Engine::ColoredTriangle(mathTriangle, triangleMaterial),
	};
	//----------------------------------------------------------------------------------

	//meshes
	//----------------------------------------------------------------------------------
	Engine::Material meshMaterial = Engine::Material({ 0.1745, 0.01175, 0.01175 }, { 0.61424,	0.04136	,0.04136 }, { 0.727811,	0.626959, 0.626959 }, 0.6);
	
	glm::mat4 tMatCube1
	{
		{4,0,0,0},
		{0,2,0,0},
		{0,0,2,0},
		{5,0,-5,1},
	};

	glm::mat4 tMatCube2
	{
		{0.8,0.6,0,0},
		{-0.6,0.8,0,0},
		{0,0,1,0},
		{0,5,0,1},
	};

	Engine::Cube::box->min = glm::vec3(-0.5f, -0.5f, -0.5f);
	Engine::Cube::box->max = glm::vec3(0.5f, 0.5f, 0.5f);

	std::vector<Engine::ColorCube> meshes
	{
		Engine::ColorCube(tMatCube1, meshMaterial),
		Engine::ColorCube(tMatCube2, meshMaterial),
	};
	//----------------------------------------------------------------------------------

	//lights
	//----------------------------------------------------------------------------------
	//direction light
	std::vector<Engine::ColorDirectionLight> directionalLights
	{
		Engine::ColorDirectionLight(glm::vec3(-1, -1, -1), glm::vec3(0.1f, 0.1f, 0.1f)),
		//Engine::ColorDirectionLight(glm::vec3(-1, -1, -1), glm::vec3(0.5f, 0.5f, 0.5f)),

	};
	//pointLight
	std::vector<Engine::ColorPointLight> pointLights
	{
		Engine::ColorPointLight(Engine::Sphere(glm::vec3(-10, 5, 5), 0.5), glm::vec3(0.5, 0.5 , 0)),
	};
	//spotLight
	std::vector<Engine::ColorSpotLight> spotLight
	{
		Engine::ColorSpotLight(Engine::Sphere(glm::vec3(20,10,5), 0.5), glm::vec3(0,-1,0), glm::radians(20.f), glm::radians(15.0f), glm::vec3(0,0,0.5)),
	};
	//----------------------------------------------------------------------------------


	//scene
	//----------------------------------------------------------------------------------
	scene = std::make_shared<Engine::Scene>(Engine::Scene(spheres, planes, triangles, meshes, directionalLights, pointLights, spotLight));
	//----------------------------------------------------------------------------------


	//camera
	//----------------------------------------------------------------------------------
	camera = std::make_shared<Engine::Camera>(Engine::Camera());
	glm::vec3 cameraRight{ glm::cross(worldUp, cameraForward) };
	glm::vec3 cameraUp{ glm::cross(cameraForward, cameraRight) };
	camera->setView(cameraPosition, glm::normalize(cameraForward), glm::normalize(cameraUp));
	camera->setPerspective(cameraFov, cameraAspectRatio, cameraNearZ, cameraFarZ);

	//----------------------------------------------------------------------------------


	//window 
	//----------------------------------------------------------------------------------
	WindowSetup(L"Ray caster", 300, 300, ScreenWidth, ScreenHeight, appHandle, windowShowParams);
	//----------------------------------------------------------------------------------


}

void Application::Run()
{

	while (true)
	{
		double timeElapsed = fpsTimer.frameTimeElapsed();

		if (timeElapsed > fpsTimer.oneFPS) // enter the scope not more than 60 times per second
		{
			if (!ProcessInputs())
			{
				return;
			}
			scene->render(window, camera, lastMousePosition);
			window.flush();
			fpsTimer.resetClock();
		}
		std::this_thread::yield();
	}
}

bool Application::ProcessInputs()
{
	float xPos{ lastMousePosition.x };
	float yPos{ lastMousePosition.y };
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (pitchYawRotation || captureObj)
			{
				xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
				yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			}
			break;
		case WM_LBUTTONDOWN:
			xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
			yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			PitchYawRotate(xPos, yPos);
			break;
		case WM_LBUTTONUP:
			EndRotate();
			break;
		case WM_RBUTTONDOWN:
			xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
			yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			captureObj = true;
			initMousePosition = lastMousePosition = glm::vec2(xPos, yPos);
			break;
		case WM_RBUTTONUP:
			captureObj = false;
			lastMousePosition = initMousePosition = glm::vec2(-1.f);
			scene->mover.release();
			break;
		case WM_KEYDOWN:
			switch (msg.wParam)
			{
			case 0x57: // W
				OnMove(camera->forward());
				break;
			case 0x41: // A
				OnMove(-camera->right());
				break;
			case 0x53: // S
				OnMove(-camera->forward());
				break;
			case 0x44: // D
				OnMove(camera->right());
				break;
			case 0x51: // Q
				// roll left
				RollRotate(rotationSpeed * rollRotationCoeff);
				break;
			case 0x45: // E
				//roll right
				RollRotate(-rotationSpeed * rollRotationCoeff);
				break;
			case VK_CONTROL:
				OnMove(-camera->up());
				break;
			case VK_SPACE:
				OnMove(camera->up());
				break;
			case VK_SHIFT:
				OnShiftPressed();
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.wParam)
			{
			case VK_SHIFT:
				OnShiftReleased();
				break;
			case 0x57: // W
			case 0x41: // A	
			case 0x53: // S
			case 0x44: // D
			case VK_CONTROL:
			case VK_SPACE:
				EndMove();
				break;

			case 0x51: // Q
			case 0x45: // E
				EndRotate();
				break;
			}
			break;
		case WM_MOUSEWHEEL:
			if (GET_WHEEL_DELTA_WPARAM(msg.wParam) > 0)
			{
				cameraSpeed += cameraSpeed * 0.05;
			}
			else
			{
				cameraSpeed -= cameraSpeed * 0.05;
			}
			break;
		case WM_QUIT:
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		//check for change window size
		if ((float)window.GetClientWidth() / window.GetClientHeight() != cameraAspectRatio)
		{
			cameraAspectRatio = (float)window.GetClientWidth() / window.GetClientHeight();
			camera->setPerspective(cameraFov, cameraAspectRatio, cameraNearZ, cameraFarZ);
		}

	}

	float deltaTime{ static_cast<float>(fpsTimer.frameTimeElapsed()) };
	if (captureObj)
	{
		DragObject(xPos, yPos, deltaTime);
	}
	if (rollRotation)
	{
		RotateCamera(deltaTime);
	}
	if (pitchYawRotation)
	{
		PitchYawRotate(xPos, yPos);
		RotateCamera(deltaTime);
	}
	if (cameraMove)
	{
		MoveCamera(deltaTime);
	}


	return true;


}



void Application::OnMove(const glm::vec3& dir)
{
	cameraMove = true;
	cameraDir = dir;
}

void Application::EndMove()
{
	cameraMove = false;
	cameraDir = glm::vec3(0);
}

void Application::DragObject(float x, float y, float deltaTime)
{
	float xOffset{ (x - lastMousePosition.x)  };
	float yOffset{ (y - lastMousePosition.y)  };

	// window offset to world offset 
	float xWorldOffset{ xOffset * nearPlaneWidth  / window.GetClientWidth()};
	float yWorldOffset{ yOffset * nearPlaneHeight / window.GetClientHeight()};


	if (scene->mover != nullptr)
	{
		//move paralel to camera
		if (cameraMove)
		{
			scene->mover->move(cameraDir * cameraSpeed * deltaTime);
		}
		//drag along plane
		else if (xOffset != 0 || yOffset != 0)
		{
			float ratio{ glm::length(scene->mover->intersectionPoint - camera->position()) / scene->mover->cameraToNearPlane};

			xWorldOffset = ratio * xWorldOffset * dragCoef;
			yWorldOffset = ratio * yWorldOffset * dragCoef;

			glm::vec3 delta{ xWorldOffset * camera->right() - yWorldOffset * camera->up() };
			scene->mover->intersectionPoint += delta;
			scene->mover->move(delta);
		}
	}

	lastMousePosition = glm::vec2(x, y);

}

void Application::MoveCamera(float deltaTime)
{
	camera->addRelativeOffset(cameraDir * cameraSpeed * deltaTime);
}

void Application::RotateCamera(float deltaTime)
{
		camera->setWorldAngles(Engine::Angles(cameraAngles.x * deltaTime, cameraAngles.y * deltaTime, cameraAngles.z * deltaTime));
}

void Application::RollRotate(float roll)
{
	rollRotation = true;
	cameraAngles.z = roll;
}

void Application::PitchYawRotate(float x, float y)
{
	if (!pitchYawRotation)
	{
		initMousePosition = lastMousePosition = glm::vec2(x, y);
		pitchYawRotation = true;
		return;
	}
	float offsetYaw{ };
	float offsetPitch{ };

	bool yawConstRotation{ std::abs(x - initMousePosition.x) >= (float)window.GetClientWidth() / 2 };
	bool pitchConstRotation{ std::abs(y - initMousePosition.y) >= (float)window.GetClientHeight() / 2 };


	float miliToSec{ 0.001f };
	if (yawConstRotation)
	{
		if (x - initMousePosition.x > 0)
		{
			offsetYaw = glm::radians(180.f) * miliToSec;
		}
		else
		{
			offsetYaw = glm::radians(-180.f) * miliToSec;
		}
	}
	if (pitchConstRotation)
	{
		if (y - initMousePosition.y > 0)
		{
			offsetPitch = glm::radians(180.f) * miliToSec;
		}
		else
		{
			offsetPitch = glm::radians(-180.f) * miliToSec;
		}

	}
	if (!pitchConstRotation && !yawConstRotation)
	{
		offsetYaw = (x - lastMousePosition.x) * rotationSpeed;
		offsetPitch = (y - lastMousePosition.y) * rotationSpeed;
	}



	lastMousePosition = glm::vec2(x, y);
	cameraAngles = glm::vec3(offsetPitch, offsetYaw, cameraAngles.z);
}

void Application::EndRotate()
{
	lastMousePosition = initMousePosition = glm::vec2(-1.f);
	rollRotation = false;
	pitchYawRotation = false;
	cameraAngles = glm::vec3(0);
}



void Application::OnShiftPressed()
{
	if (!increasingSpeed)
	{
		increasingSpeed = true;
		cameraSpeed *= 5;

	}
}

void Application::OnShiftReleased()
{
	if (increasingSpeed)
	{
		increasingSpeed = false;
		cameraSpeed /= 5;
	}

}




void Application::Close()
{
	ExitProcess(0);
}
