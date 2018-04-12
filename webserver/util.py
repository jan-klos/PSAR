import os, shutil, settings

def get_file_size(file):
    return os.path.getsize(file)

# extensions not exhaustive; test purpose
def get_file_type(file):
    file_extension = os.path.splitext(file)[1]
    if file_extension in (".png", ".jpg"):
        return "img"
    elif file_extension in (".mp3", ".flac"):
        return "music"
    elif file_extension in (".mp4", ".avi", ".webm", ".gif"):
        return "video"     
    else:
        return "txt"

def get_file_dict(file_name):
    return { "type" : get_file_type(file_name), "name" : file_name, "size" : get_file_size(settings.files_path + file_name), 
             "path" : settings.files_path + file_name }

def get_file_dict_list(file_list):
    file_dict_list = []
    for file in file_list:
        file_dict_list.append(get_file_dict(file))
    return file_dict_list

def create_file(file_name, file_content):    
    file = open(settings.files_path + file_name, "w+")
    file.write(file_content)

def get_file_content(file_name):
    with open(settings.files_path + file_name) as file:
        file_content = file.read().replace("\n", "<br>")
    return file_content

def copy_file_to_static(file_name):
    shutil.copyfile(settings.files_path + file_name, settings.current_path + "/static/img/" + file_name)

def create_static(path):
    if not os.path.exists(settings.current_path + "/static/img/"):
        os.makedirs(settings.current_path + "/static/img/")

def clear_static():
    shutil.rmtree(settings.current_path + "/static/img/")
    create_static("/static/img")