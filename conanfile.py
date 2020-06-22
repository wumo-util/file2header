from conans import ConanFile, CMake, tools

class file2headerConan(ConanFile):
  name = "file2header"
  version = "1.0.5"
  settings = "os", "compiler", "build_type", "arch"
  requires = ()
  generators = "cmake"
  scm = {
    "type": "git",
    "subfolder": name,
    "url": "auto",
    "revision": "auto"
  }

  def build(self):
    cmake = CMake(self)
    cmake.configure(source_folder=self.name)
    cmake.build()

  def imports(self):
    self.copy("*.dll", dst="bin", src="bin")
    self.copy("*.dll", dst="bin", src="lib")
    self.copy("*.dylib*", dst="bin", src="lib")
    self.copy("*.pdb", dst="bin", src="bin")
    self.copy("*", dst="bin/assets/public", src="resources")

  def package(self):
    self.copy("*", dst="bin", src="bin", keep_path=False)
    self.copy("*.h", dst="include", src=f"{self.name}/src")
    self.copy("*.so", dst="lib", src="lib", keep_path=False)
    self.copy("*.a", dst="lib", src="lib", keep_path=False)
    self.copy("*.lib", dst="lib", src="lib", keep_path=False)
    self.copy("resources.cmake", dst="cmake", src=f"{self.name}/cmake")

  def package_info(self):
    self.cpp_info.build_modules.append("cmake/resources.cmake")
    self.cpp_info.libs = tools.collect_libs(self)
