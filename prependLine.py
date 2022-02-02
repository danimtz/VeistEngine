

new_line = '#include "pch.h"\n'


import glob
root_dir = "Engine/"

for file in glob.iglob(root_dir + '**/*.cpp', recursive=True):
    with open(file, 'r+') as file:
        print(file)
        content = file.read()
        file.seek(0)
        file.write(new_line + content)


