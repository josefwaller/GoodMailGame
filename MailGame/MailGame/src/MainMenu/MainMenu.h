#pragma once
#include <string>
class App;

class MainMenu {
public:
	MainMenu(App* app);
	void update();
private:
	App* app;
	std::string isDeleting;
	bool isSure;
};
