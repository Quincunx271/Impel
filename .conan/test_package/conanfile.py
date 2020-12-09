from conans import ConanFile, CMake
import os


class ConanTestPackage(ConanFile):
    generators = 'cmake'

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        self.run('./test_package', run_environment=True)
