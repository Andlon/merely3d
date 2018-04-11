#pragma once

namespace merely3d
{
    /// Convenience class that calls glfwInit() upon construction
    /// and glfwTerminate() upon destruction.
    ///
    /// In addition, it registers an error callback with GLFW and
    /// output more detailed error description to stderr whenever a GLFW
    /// error occurs.
    class App final
    {
    public:
        App();
        ~App();
    };
}