from django.urls import path

from . import views

urlpatterns = [
    path("", views.index, name="index"),
    path("add_new_file", views.add_new_file, name="add_new_file"),
    path("add_new_folder", views.add_new_folder, name="add_new_folder"),
    path("delete_file_or_folder", views.delete_file_or_folder, name="delete_file_or_folder"),
    path("login", views.login, name="login"),
    path("register", views.register, name="register"),
    path("logout", views.logout, name="logout"),
    path("save_file", views.save_file, name="save_file"),
    path("get_file", views.get_file, name="get_file"),
    path("add_section", views.add_section, name="add_section"),
]