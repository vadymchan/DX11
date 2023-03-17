#pragma once



namespace engine::DX
{

	class TextureManager
	{
	public:

		static TextureManager& getInstance()
		{
			static TextureManager instance;
			return instance;
		}

	private:

	};

}