from django.test import TestCase
from django.db import IntegrityError
from django.db import transaction
from .models import *
from django.shortcuts import reverse
from django.contrib.auth import login as login_auth
from django.test.client import RequestFactory
from .views import *
from django.contrib.auth.models import AnonymousUser
import json

def create_testuser():
    base_user = User.objects.create_user(username="__TEST_USER__", password="password")
    base_user.save()
    user = ExtendedUser(base=base_user, name="__TEST_USER_USERNAME__")
    user.save()
    return user

def delete_testuser(user):
    base_user = user.base
    base_user.delete()
    user.delete()

class FolderTests(TestCase):
    def test_contraint_folder(self):
        correct = False
        extended_user = create_testuser()
        user = extended_user.base

        folder_parent = Folder(name="parent", owner=user, parent_dir=None)
        folder_parent.save()
        folder_one = Folder(name="TEST_FOLDER", owner=user, parent_dir=folder_parent)
        folder_one.save()
        folder_two = Folder(name="TEST_FOLDER", owner=user, parent_dir=folder_parent)
        try:
            with transaction.atomic():
                folder_two.save()
        except IntegrityError:
            correct = True
        finally:
            folder_one.delete()
            if not correct:
                folder_two.delete()
            delete_testuser(extended_user)

            self.assertEqual(correct, True)

class FileTests(TestCase):
    def test_contraint_file(self):
        correct = False
        extended_user = create_testuser()
        user = extended_user.base

        folder_parent = Folder(name="parent", owner=user, parent_dir=None)
        folder_parent.save()
        file_one = File(name="TEST_FILE", owner=user, parent_dir=folder_parent)
        file_one.save()
        file_two = File(name="TEST_FILE", owner=user, parent_dir=folder_parent)
        try:
            with transaction.atomic():
                file_two.save()
        except IntegrityError:
            correct = True
        finally:
            file_one.delete()
            if not correct:
                file_two.delete()
            delete_testuser(extended_user)

            self.assertEqual(correct, True)

class AddFileTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base

    def test_correct_data(self):
        data = {
            "path": "src/uno/dos/tres/plik.c",
            "file_content" : "",
        }
        request = self.factory.post("add_new_file", data)
        request.user = self.user
        response = add_new_file(request)

        user = request.user
        correct = True
        self.assertEqual(File.objects.filter(name="plik.c", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="tres", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="dos", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="uno", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="src", owner=user).count(), 1)

        src = Folder.objects.get(name="src", owner=user)
        uno = Folder.objects.get(name="uno", owner=user)
        dos = Folder.objects.get(name="dos", owner=user)
        tres = Folder.objects.get(name="tres", owner=user)
        file = File.objects.get(name="plik.c", owner=user)
        
        self.assertEqual(file.parent_dir, tres)
        self.assertEqual(tres.parent_dir, dos)
        self.assertEqual(dos.parent_dir, uno)
        self.assertEqual(uno.parent_dir, src)

        file.delete()
        tres.delete()
        dos.delete()
        uno.delete()
        src.delete()

    def test_missing_content(self):
        data = {
            "path": "plik.c",
        }
        request = self.factory.post("add_new_file", data)
        request.user = self.user
        response = add_new_file(request)

        self.assertEqual(File.objects.filter(name="plik.c").count(), 0)

    def test_missing_path(self):
        data = {
            "file_content": "",
        }
        request = self.factory.post("add_new_file", data)
        request.user = self.user
        response = add_new_file(request)

        self.assertEqual(File.objects.filter(name="plik.c").count(), 0)
    
    def test_wrong_format_one(self):
        data = {
            "path": "src//plik.c",
            "file_content" : "",
        }
        request = self.factory.post("add_new_file", data)
        request.user = self.user
        response = add_new_file(request)

        self.assertEqual(File.objects.filter(name="plik.c").count(), 0)
    
    def test_wrong_format_two(self):
        data = {
            "path": "/src/plik.c",
            "file_content" : "",
        }
        request = self.factory.post("add_new_file", data)
        request.user = self.user
        response = add_new_file(request)

        self.assertEqual(File.objects.filter(name="plik.c").count(), 0)
    
    def test_wrong_format_three(self):
        data = {
            "path": "src/plik.c/",
            "file_content" : "",
        }
        request = self.factory.post("add_new_file", data)
        request.user = self.user
        response = add_new_file(request)

        self.assertEqual(File.objects.filter(name="plik.c").count(), 0)

    def test_file_already_existing(self):
        data = {
            "path": "plik.c",
            "file_content" : "",
        }

        request = self.factory.post("add_new_file", data)
        request.user = self.user

        f = File(name="plik.c", owner=self.user, parent_dir=None)
        f.save()

        response = add_new_file(request)

        f.delete()

        self.assertEqual(File.objects.filter(name="plik.c").count(), 0)
    


class AddFolderTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base

    def test_correct_data(self):
        data = {
            "path": "src/uno/dos/tres",
        }
        request = self.factory.post("add_new_folder", data)
        request.user = self.user
        response = add_new_folder(request)

        user = request.user
        correct = True
        self.assertEqual(Folder.objects.filter(name="tres", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="dos", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="uno", owner=user).count(), 1)
        self.assertEqual(Folder.objects.filter(name="src", owner=user).count(), 1)

        src = Folder.objects.get(name="src", owner=user)
        uno = Folder.objects.get(name="uno", owner=user)
        dos = Folder.objects.get(name="dos", owner=user)
        tres = Folder.objects.get(name="tres", owner=user)
        
        self.assertEqual(tres.parent_dir, dos)
        self.assertEqual(dos.parent_dir, uno)
        self.assertEqual(uno.parent_dir, src)

        tres.delete()
        dos.delete()
        uno.delete()
        src.delete()
    def test_missing_path(self):
        data = {}
        request = self.factory.post("add_new_folder", data)
        request.user = self.user
        response = add_new_folder(request)

        self.assertEqual(Folder.objects.filter(name="src").count(), 0)
    
    def test_wrong_format_one(self):
        data = {
            "path" : "src/uno//dos",
        }
        request = self.factory.post("add_new_folder", data)
        request.user = self.user
        response = add_new_folder(request)

        self.assertEqual(Folder.objects.filter(name="dos").count(), 0)
    
    def test_wrong_format_two(self):
        data = {
            "path" : "/src/uno/dos",
        }
        request = self.factory.post("add_new_folder", data)
        request.user = self.user
        response = add_new_folder(request)

        self.assertEqual(Folder.objects.filter(name="dos").count(), 0)
    
    def test_wrong_format_three(self):
        data = {
            "path" : "src/uno/dos/",
        }
        request = self.factory.post("add_new_folder", data)
        request.user = self.user
        response = add_new_folder(request)

        self.assertEqual(Folder.objects.filter(name="dos").count(), 0)

    def test_folder_already_existing(self):
        data = {
            "path": "src",
        }

        request = self.factory.post("add_new_folder", data)
        request.user = self.user

        f = Folder(name="src", owner=self.user, parent_dir=None)
        f.save()

        response = add_new_folder(request)

        f.delete()

        self.assertEqual(Folder.objects.filter(name="src").count(), 0)


class DeleteFileFolderTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base

    def test_correct_delete_file(self):
        f1 = Folder(name="one", owner=self.user)
        f1.save()
        f2 = Folder(name="two", owner=self.user, parent_dir=f1)
        f2.save()
        f3 = File(name="plik", owner=self.user, parent_dir=f2)
        f3.save()


        data = {
            "path": "one/two/plik",
        }
        request = self.factory.post("delete_file_or_folder", data)
        request.user = self.user
        response = delete_file_or_folder(request)

        self.assertEqual(File.objects.get(name="plik").accessible, False)
    
    def test_correct_delete_folder(self):
        f1 = Folder(name="one", owner=self.user)
        f1.save()
        f2 = Folder(name="two", owner=self.user, parent_dir=f1)
        f2.save()
        f3 = Folder(name="three", owner=self.user, parent_dir=f2)
        f3.save()


        data = {
            "path": "one/two/three",
        }
        request = self.factory.post("delete_file_or_folder", data)
        request.user = self.user
        response = delete_file_or_folder(request)

        self.assertEqual(Folder.objects.get(name="three").accessible, False)

class SaveFileTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base
    def test_save_file_correct(self):
        f1 = File(name="TEST_FILE", owner=self.user, parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
            "code_editor": "new content",
        }
        request = self.factory.post("save_file", data)
        response = save_file(request)
        f1 = File.objects.get(id=id)
        self.assertEqual(f1.content, "new content")
    def test_save_file_incomplete(self):
        f1 = File(name="TEST_FILE", owner=self.user, content="old", parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
        }
        request = self.factory.post("save_file", data)
        response = save_file(request)
        f1 = File.objects.get(id=id)
        self.assertEqual(f1.content, "old")

class GetFileTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base
    def test_save_file_correct(self):
        f1 = File(name="TEST_FILE", owner=self.user, content="content", parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
        }
        request = self.factory.post("get_file", data)
        response = get_file(request)
        response = json.loads(response.content)
        rec_id = response["id"]
        rec_content = response["content"]
        rec_name = response["name"]
        self.assertEqual(str(f1.id), rec_id)
        self.assertEqual(f1.content, rec_content)
        self.assertEqual(f1.name, rec_name)

class AddSectionTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base
    def test_add_section_correct(self):
        f1 = File(name="TEST_FILE", owner=self.user, content="content", parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
            "start": "0",
            "end": "2",
            "type": "comment",
            "name": "section name",
            "description": "section description",
        }
        request = self.factory.post("add_section", data)
        request.user = self.user
        response = add_section(request)
        section = Section.objects.get(file=f1)
        self.assertEqual(section.start_char, 0)
        self.assertEqual(section.end_char, 2)
        self.assertEqual(section.name, "section name")
        self.assertEqual(section.description, "section description")
        self.assertEqual(section.section_type, Section.Type.COMMENT)
    
    def test_add_section_missing_mandatory(self):
        f1 = File(name="TEST_FILE", owner=self.user, content="content", parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
            "start": "0",
            "end": "2",
            "name": "section name",
            "description": "section description",
        }
        request = self.factory.post("add_section", data)
        request.user = self.user
        response = add_section(request)
        self.assertEqual(Section.objects.filter(file=f1).count(), 0)
    
    def test_add_section_wrong_fragment_bounds(self):
        f1 = File(name="TEST_FILE", owner=self.user, content="content", parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
            "start": "2",
            "end": "1",
            "name": "section name",
            "type": "comment",
            "description": "section description",
        }
        request = self.factory.post("add_section", data)
        request.user = self.user
        response = add_section(request)
        self.assertEqual(Section.objects.filter(file=f1).count(), 0)
    
    def test_add_section_fragment_bounds_outside_content(self):
        f1 = File(name="TEST_FILE", owner=self.user, content="content", parent_dir=None)
        f1.save()
        id = f1.id
        data = {
            "file_id": str(f1.id),
            "start": "2",
            "end": "20",
            "name": "section name",
            "type": "comment",
            "description": "section description",
        }
        request = self.factory.post("add_section", data)
        request.user = self.user
        response = add_section(request)
        self.assertEqual(Section.objects.filter(file=f1).count(), 0)

class IndexTests(TestCase):
    def setUp(self):
        self.factory = RequestFactory()
        user = create_testuser()
        self.user = user.base
    
    def test_make_structure(self):
        user = self.user
        folder_a = Folder(name="folder_a", owner=user, parent_dir=None)
        folder_a.save()
        folder_b = Folder(name="folder_b", owner=user, parent_dir = folder_a)
        folder_b.save()
        file_a = File(name="file_a", owner=user, parent_dir=folder_a)
        file_a.save()
        file_b = File(name="file_b", owner=user, parent_dir=folder_b)
        file_b.save()
        request = self.factory.post("make_project_structure", None)
        request.user = user
        response = make_project_structure(request)

        self.assertEqual(len(response["folders"]), 1)
        fol_a = response["folders"][0]
        self.assertEqual(fol_a["folder"].id, folder_a.id)

        self.assertEqual(len(fol_a["folders"]), 1)
        self.assertEqual(len(fol_a["files"]), 1)

        fil_a = fol_a["files"][0]
        self.assertEqual(fil_a.id, file_a.id)

        fol_b = fol_a["folders"][0]
        self.assertEqual(len(fol_b["folders"]), 0)
        self.assertEqual(len(fol_b["files"]), 1)

        fil_b = fol_b["files"][0]
        self.assertEqual(fil_b.id, file_b.id)

