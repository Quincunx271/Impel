#!/usr/bin/env python3

import os

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    env = os.environ

    username = env['CONAN_USERNAME']
    login_username = env.get('CONAN_LOGIN_USERNAME', username)
    version = env['CONAN_VERSION']
    channel = env['CONAN_CHANNEL']
    upload = env.get('CONAN_UPLOAD', 'https://api.bintray.com/conan/quincunx271/public/')

    args = {
        'username': username,
        'login_username': login_username,
        'reference': f'impel/{version}@{username}/{channel}',
        'test_folder': os.path.join('.conan', 'test_package'),
    }
    if 'SHOULD_UPLOAD_CONAN' in env:
        args['upload'] = upload

    builder = ConanMultiPackager(**args)
    builder.add()
    builder.run()
