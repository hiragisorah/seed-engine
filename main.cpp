#include <stdio.h>

#include <crtdbg.h>

#include <seed-engine\engine.h>

#include <window\simple-window.h>
#include <graphics\directx11.h>
#include <scene\test-scene.h>

#ifdef _DEBUG
#define DEBUG
void main(void)
{
	// メモリーリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
int __stdcall WinMain(HINSTANCE, HINSTANCE, char*, int)
#endif

	Seed::Engine engine;

	engine.set_window<SimpleWindow>("test", 1280U, 720U);
	engine.set_graphics<DirectX11>();

	engine.Run<TestScene>();
}