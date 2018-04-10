from flask import Flask, render_template, request
import os, util, settings

app = Flask(__name__)

if not os.path.exists(settings.path):
    os.makedirs(settings.path)

# folder for temporary image files storing
if not os.path.exists("static/img/"):
    os.makedirs("static/img/")

@app.route("/")
def main():
    util.clear_static() # removes temporary image files
    file_list = os.listdir(settings.path)
    file_dict_list = util.get_file_dict_list(file_list)
    template_data = { "files" : file_dict_list }
    return render_template("main.html", **template_data)
    
@app.route("/", methods = ["POST"])
def create_file():
    file_name =  request.form["file_name"]
    file_content = request.form["file_content"]
    util.create_file(file_name, file_content)	
    return main()

@app.route("/<file_name>", methods = ["GET"])
def show_file(file_name):
    file_dict = util.get_file_dict(file_name)
    if file_dict["type"] == "img":
        util.copy_file_to_static(file_name)
        template_data = { "file" : file_dict}
    else:
        template_data = { "file" : file_dict, "file_content" : util.get_file_content(file_name) }
    return render_template("file.html", **template_data)
    
if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 80, debug = True)
