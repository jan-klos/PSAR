import os, shutil, settings
from werkzeug.utils import secure_filename
import ctypes

def get_file_size(file):
    return os.path.getsize(file)


# extensions not exhaustive; test purpose
def get_file_type(file):
    file_extension = os.path.splitext(file)[1]
    if file_extension in (".png", ".jpg", "jpeg"):
        return "img"
    elif file_extension in (".mp3", ".flac"):
        return "music"
    elif file_extension in (".mp4", ".avi", ".webm", ".gif"):
        return "video"
    elif file_extension == ".pdf":
        return "pdf"
    elif file_extension in (".txt", ""):
        return "txt"


def get_file_dict(file_name):
    return {"type": get_file_type(file_name), "name": file_name, "size": get_file_size(settings.FILES_PATH + file_name),
            "path": settings.FILES_PATH + file_name}


def get_file_dict_list(file_list):
    file_dict_list = []
    for file in file_list:
        file_dict_list.append(get_file_dict(file))
    return file_dict_list


def create_file(file_name, file_content):
    with open(settings.FILES_PATH + file_name, "w+") as file:
        file.write(file_content)


def upload_file(file):
    if file and is_allowed_extension(file.filename):
        filename = secure_filename(file.filename)
        file.save(os.path.join(settings.FILES_PATH, filename))


def get_file_content(file_name):
    with open(settings.FILES_PATH + file_name) as file:
        file_content = file.read().replace("\n", "<br>")
    return file_content


def copy_file_to_tmp(file_name):
    shutil.copyfile(settings.FILES_PATH + file_name, settings.CURRENT_PATH + "/static/tmp/" + file_name)


def create_tmp():
    if not os.path.exists(settings.CURRENT_PATH + "/static/tmp/"):
        os.makedirs(settings.CURRENT_PATH + "/static/tmp/")


def clear_tmp():
    shutil.rmtree(settings.CURRENT_PATH + "/static/tmp/")
    create_tmp()


def is_allowed_extension(file_name):
    return '.' in file_name and file_name.rsplit('.', 1)[1].lower() in settings.ALLOWED_EXTENSIONS


def write_send_info(send_address, file_name):
    with open(settings.SEND_FILES_PATH, "a+") as file:
        file.write(send_address + "\n" + settings.FILES_PATH + file_name + "\n")


lib = ctypes.cdll.LoadLibrary('../protocol/libprotocol.so')
class Protocol(object):
    interface = "wlan1"

    def __init__(self):
        self.obj = lib.Protocol_new()

    # Spray & Wait
    def send_file(self, file_path, address_dest):
        lib.Protocol_send_file(self.obj, 
            ctypes.c_char_p(self.interface),
            ctypes.c_char_p(settings.FILES_PATH + file_path), 
            ctypes.c_char_p(str(address_dest)))

    # Epidemic
    def sync_files(self):
        lib.Protocol_sync_files(self.obj, 
            ctypes.c_char_p(self.interface))