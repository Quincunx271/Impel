from conans import ConanFile, CMake, tools


class ImpelConanFile(ConanFile):
    name = 'impel'
    url = 'https://github.com/Quincunx271/Impel'
    license = 'BSL-1.0'
    no_copy_source = True
    generators = 'cmake'

    build_requires = (
        'Catch2/2.5.0@catchorg/stable',
    )
    exports_sources = 'pmm.cmake', 'cmake/*', 'include/*', 'CMakeLists.txt', 'LICENSE.txt'

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(defs={
            'BUILD_TESTING': False,
        })
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
        self.copy('LICENSE.txt', 'licenses')
