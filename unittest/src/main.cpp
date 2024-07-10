#include <gtest/gtest.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "xgl/xgl.h"

class TestEnvironment : public testing::Environment {
public:
	void SetUp() override {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		_window = glfwCreateWindow(640, 480, "unittest", nullptr, nullptr);
		glfwMakeContextCurrent(_window);
		gladLoadGL();
	}

	void TearDown() override {
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

private:
	GLFWwindow* _window{};
};

class TestCaseListener : public testing::EmptyTestEventListener {
public:
	void OnTestStart(const testing::TestInfo& test_info) override {
	}

	void OnTestEnd(const testing::TestInfo& test_info) override {
	}
};


int main(int argc, char** argv) {
	using namespace testing;
	InitGoogleTest(&argc, argv);
	AddGlobalTestEnvironment(new TestEnvironment);

	TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
	listeners.Append(new TestCaseListener);

	return RUN_ALL_TESTS();
}
