from flask import Flask, render_template, request, redirect, flash
import os, util, settings

app = Flask(__name__)

if not os.path.exists(settings.FILES_PATH):
    os.makedirs(settings.FILES_PATH)

# folder for temporary image files storing
util.create_static("/static/img")

@app.route("/")
def main():
    util.clear_static() # removes temporary image files
    file_list = os.listdir(settings.FILES_PATH)
    file_dict_list = util.get_file_dict_list(file_list)
    template_data = { "files" : file_dict_list }
    return render_template("main.html", **template_data)
    
@app.route("/", methods = ["POST"])
def add_file():
    if "create_file_submit" in request.form and request.form["file_name"] != "":
        util.create_file(request.form["file_name"], request.form["file_content"])
    elif "upload_file_submit" in request.form and request.files["file"].filename != "":
        util.upload_file(request.files["file"])
    return main()

@app.route("/files/<file_name>", methods = ["POST"])
def send_file(file_name):
    send_address = request.form["send_address"]
    #proxy
    print "send_file(): \n    file_name: {0} \n    address: {1}".format(file_name, send_address)
    #proxy
    return redirect("/", code = 302)

@app.route("/files/<file_name>", methods = ["GET"])
def show_file(file_name):
    file_dict = util.get_file_dict(file_name)
    if file_dict["type"] in ("img", "pdf"):
        util.copy_file_to_static(file_name)
        template_data = { "file" : file_dict}
    else:
        template_data = { "file" : file_dict, "file_content" : util.get_file_content(file_name) }
    return render_template("file.html", **template_data)

if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 80, debug = True)
