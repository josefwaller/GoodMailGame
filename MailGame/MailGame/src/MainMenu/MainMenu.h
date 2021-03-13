#pragma once
class App;

class MainMenu {
public:
	MainMenu(App* app);
	void update();
private:
	App* app;
};
