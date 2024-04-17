from django.contrib import admin
from .models import *


admin.site.register(Folder)
admin.site.register(File)
admin.site.register(Section)
admin.site.register(ExtendedUser)