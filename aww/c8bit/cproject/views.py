from django.shortcuts import render, redirect
from django.http import HttpResponse, JsonResponse
from django.contrib.auth import authenticate, logout as auth_logout, login as auth_login
from .models import *
import datetime
import subprocess
import re
from django.contrib import messages

def make_project_structure(request):
    def build_structure(project_structure, user):
        our_folders = Folder.objects.filter(parent_dir=project_structure["folder"], accessible=True, owner=user)
        our_files = File.objects.filter(parent_dir=project_structure["folder"], accessible=True, owner=user)
        project_structure["files"] = our_files
        for f in our_folders:
            project_structure["folders"].append({
                "folder" : f,
                "folders" : [],
                "files" : [],
            })
        for f in project_structure["folders"]:
            build_structure(f, user)
    project_structure = {
        "folders" : [],
        "files" : None,
    }
    user = None
    if request.user.is_authenticated:
        user = request.user
    else:
        return project_structure
    oldest_folders = Folder.objects.filter(parent_dir__isnull=True, accessible=True, owner=user)
    oldest_files = File.objects.filter(parent_dir__isnull=True, accessible=True, owner=user)
    project_structure["files"] = oldest_files
    for f in oldest_folders:
        project_structure["folders"].append({
            "folder" : f,
            "folders" : [],
            "files" : [],
        })
    for folder in project_structure["folders"]:
        build_structure(folder, user)
    return project_structure


def index(request):
    username = None
    if request.user.is_authenticated:
        if ExtendedUser.objects.filter(base=request.user).exists():
            username = ExtendedUser.objects.get(base=request.user).name

    project_structure = make_project_structure(request)

    id = request.POST.get("file_id")
    file_shown = None
    if id is not None:
        file_shown = File.objects.get(id=int(id))

    compilation_output = ""
    output_processed = []

    if request.POST.get("compilation") is not None:
        processor = request.POST.get("processor")
        standard = request.POST.get("c_standard")
        standard = standard.lower()
        id = int(request.POST.get("file_id"))
        file = File.objects.get(id=id)
        command = ["sdcc", "-S", "temp.c", "--std-"+standard]
        if processor == "MCS51":
            command.append("-mmcs51")
            for i in range(1, 5):
                option = request.POST.get("msc51-check-" + str(i))
                if option is not None:
                    command.append("--" + option)
        elif processor == "Z80":
            command.append("-mz80")
            for i in range(1, 5):
                option = request.POST.get("z80-check-" + str(i))
                if option is not None:
                    command.append("--" + option)
        elif processor == "STM8":
            command.append("-mstm8")
            for i in range(1, 2):
                option = request.POST.get("stm8-check-" + str(i))
                if option is not None:
                    command.append("--" + option)
        f = open("temp.c", "w")
        f.write(file.content)
        f.close()

        subprocess.run(command)
        
        f = open("temp.asm", "r")
        output = f.read()
        compilation_output = output

        # further processing of output
        lines = output.split("\n")
        cur_header = None
        cur_string = ""
        add_newline = False
        pattern = re.compile("\S+:\n")
        for line in lines:
            if pattern.match(line + "\n"):
                output_processed.append({
                    "header" : cur_header,
                    "content" : cur_string,
                })
                cur_string = ""
                cur_header = line
                add_newline = False
            else:
                if add_newline:
                    cur_string += "\n"
                cur_string += line
                add_newline = True
        output_processed.append({
            "header" : cur_header,
            "content" : cur_string,
        })

        for i in range(len(output_processed)):
            output_processed[i]["id"] = i

    context = {
        "project_structure" : project_structure,
        "file_shown" : file_shown,
        "compilation_output" : compilation_output,
        "processed_output" : output_processed,
        "username" : username,
    }

    return render(request=request, context=context, template_name="main.html")

def add_new_file(request):
    if not request.user.is_authenticated:
        return redirect("index")
    path = request.POST.get("path")
    content = request.POST.get("file_content")
    if path is None or content is None:
        return redirect("index")
    path = path.split("/")
    for name in path:
        if name == "":
            return redirect("index")
    
    prev = None
    length = len(path)
    if length == 1:
        if File.objects.filter(owner=request.user, parent_dir=None, accessible=True, name=path[length - 1]).count() > 0:
            return redirect("index")
        file = File(owner=request.user, parent_dir=None, accessible=True, name=path[0],
                creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now(),
                content=content)
        file.save()
        return redirect("index")
    if Folder.objects.filter(owner=request.user, parent_dir__isnull=True, accessible=True, name=path[0]).count() > 0:
        prev = Folder.objects.filter(owner=request.user, parent_dir__isnull=True, accessible=True, name=path[0]).get()
    else:
        f = Folder(owner=request.user, parent_dir=None, accessible=True, name=path[0],
            creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now())
        f.save()
        prev = f

    for i in range (1, length - 1):
        if Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[i]).count() > 0:
            prev = Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[i]).get()
        else:
            f = Folder(owner=request.user, parent_dir=prev, accessible=True, name=path[i],
                creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now())
            f.save()
            prev = f
    
    if File.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1]).count() > 0:
        return redirect("index")
    file = File(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1],
                creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now(),
                content=content)
    file.save()

    return redirect("index")

def add_new_folder(request):
    if not request.user.is_authenticated:
        return redirect("index")
    path = request.POST.get("path")
    if path is None:
        return redirect("index")
    path = path.split("/")
    for name in path:
        if name == "":
            return redirect("index")
    prev = None
    length = len(path)

    if length == 1:
        if Folder.objects.filter(owner=request.user, parent_dir=None, accessible=True, name=path[0]).count() > 0:
            return redirect("index")
        folder = Folder(owner=request.user, parent_dir=None, accessible=True, name=path[0],
                creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now())
        folder.save()
        return redirect("index")
    if Folder.objects.filter(owner=request.user, parent_dir__isnull=True, accessible=True, name=path[0]).count() > 0:
        prev = Folder.objects.filter(owner=request.user, parent_dir__isnull=True, accessible=True, name=path[0]).get()
    else:
        f = Folder(owner=request.user, parent_dir=None, accessible=True, name=path[0],
            creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now())
        f.save()
        prev = f

    for i in range (1, length - 1):
        if Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[i]).count() > 0:
            prev = Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[i]).get()
        else:
            f = Folder(owner=request.user, parent_dir=prev, accessible=True, name=path[i],
                creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now())
            f.save()
            prev = f
    
    if Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1]).count() > 0:
        return redirect("index")
    folder = Folder(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1],
                creation_date=datetime.datetime.now(), last_change_date=datetime.datetime.now())
    folder.save()

    return redirect("index")

def delete_file_or_folder(request):
    if not request.user.is_authenticated:
        return redirect("index")
    def delete_substructure(folder):
        folders = Folder.objects.filter(parent_dir=folder)
        for f in folders:
            delete_substructure(f)
        files = File.objects.filter(parent_dir=folder)
        for f in files:
            f.accessible = False
            f.save()
        folder.accessible = False
        folder.save()
    path = request.POST.get("path")
    if path is None:
        return redirect("index")
    path = path.split("/")
    for name in path:
        if name == "":
            return redirect("index")
    prev = None
    length = len(path)
    if Folder.objects.filter(owner=request.user, parent_dir__isnull=True, accessible=True, name=path[0]).count() > 0:
        prev = Folder.objects.filter(owner=request.user, parent_dir__isnull=True, accessible=True, name=path[0]).get()
    else:
        return redirect("index")

    for i in range (1, length - 1):
        if Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[i]).count() > 0:
            prev = Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[i]).get()
        else:
            return redirect("index")
    
    if Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1]).count() > 0:
        prev = Folder.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1]).get()
        delete_substructure(prev)
    elif File.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1]).count() > 0:
        file = File.objects.filter(owner=request.user, parent_dir=prev, accessible=True, name=path[length - 1]).get()
        file.accessible = False
        file.save()
    else:
        return redirect("index")
    
    return redirect("index")

def login(request):
    if request.user.is_authenticated:
        return redirect("index")
    user_login = request.POST.get("user_login")
    password = request.POST.get("user_password")
    if user_login is not None and password is not None:
        user = authenticate(request, username=user_login, password=password)
        if user is not None:
            auth_login(request, user)
    return redirect("index")

def register(request):
    if request.user.is_authenticated:
        return redirect("index")
    user_login = request.POST.get("user_login")
    password = request.POST.get("user_password")
    username = request.POST.get("user_username")
    if user_login is not None and password is not None and username is not None:
        if User.objects.filter(username=user_login).exists():
            return redirect("index")
        base_user = User.objects.create_user(username=user_login, password=password)
        base_user.save()
        user = ExtendedUser(base=base_user, name=username)
        user.save()
        auth_login(request, base_user)
    return redirect("index")

def logout(request):
    auth_logout(request)
    return redirect("index")

def save_file(request):
    id = request.POST.get("file_id")
    new_content = request.POST.get("code_editor")
    if id is not None and new_content is not None:
        file = File.objects.get(id=int(id))
        file.content = new_content
        file.save()
        return JsonResponse({"name": file.name}, status=200)
    return JsonResponse({}, status=400)

def get_file(request):
    id = request.POST.get("file_id")
    if id is not None:
        file = File.objects.get(id=int(id))
        content = file.content
        name = file.name
        return JsonResponse({"name": file.name, "content": content, "id": id}, status=200)
    return JsonResponse({}, status=400)

def add_section(request):
    if not request.user.is_authenticated:
        return redirect("index")
    
    id = request.POST.get("file_id")
    name = request.POST.get("name")
    desc = request.POST.get("description")
    start = request.POST.get("start")
    end = request.POST.get("end")
    sec_type = request.POST.get("type")

    if id is None or start is None or end is None or sec_type is None:
        return redirect("index")
    if File.objects.filter(id=id).count == 0:
        return redirect("index")
    if int(start) > int(end):
        return redirect("index")

    file = File.objects.get(id=id)

    if int(end) >= len(file.content):
        return redirect("index")

    status = Section.Status.CORRECT

    type_option = None
    if sec_type == "procedure":
        type_option = Section.Type.PROCEDURE
    elif sec_type == "comment":
        type_option = Section.Type.COMMENT
    elif sec_type == "directive":
        type_option = Section.Type.DIRECTIVE
    elif sec_type == "variable":
        type_option = Section.Type.VARIABLE
    else:
        type_option = Section.Type.ASSEMBLER

    new_section = Section(file = file, name = name, description = desc, start_char = int(start), end_char = int(end), section_type = type_option, status = status, creation_date=datetime.datetime.now())
    new_section.save()

    return redirect("index")
