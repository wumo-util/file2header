from conans import ConanFile, CMake, tools

class file2headerConan(ConanFile):
  name = "file2header"
  version = "1.0.7"
  settings = "os", "compiler", "build_type", "arch"
  requires = ()
  generators = "cmake"
  scm = {
    "type": "git",
    "subfolder": name,
    "url": "auto",
    "revision": "auto"
  }

  def configure_cmake(self):
    cmake = CMake(self)
    cmake.configure(source_folder=self.name)
    return cmake

  def build(self):
    cmake = self.configure_cmake()
    cmake.build()

  def imports(self):
    self.copy("*.dll", dst="bin", src="bin")
    self.copy("*.dll", dst="bin", src="lib")
    self.copy("*.dylib", dst="bin", src="lib")
    self.copy("*.pdb", dst="bin", src="bin")

  def package(self):
    cmake = self.configure_cmake()
    cmake.install()
    self.copy("resources.cmake", dst="cmake", src=f"{self.name}/cmake")

  def package_info(self):
    self.cpp_info.build_modules.append("cmake/resources.cmake")
