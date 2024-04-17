from django.db import models
from django.utils.translation import gettext_lazy as _
from django.contrib.auth.models import User
from django.utils import timezone

class ExtendedUser(models.Model):
    """
    base.username - used as login
    base.password - used as password
    name - used as username
    """
    base=models.OneToOneField(User, on_delete=models.CASCADE)
    name=models.CharField(max_length=40)

class Folder(models.Model):
    name=models.CharField(max_length=100)
    description=models.TextField(blank=True)
    creation_date=models.DateField(default=timezone.now)
    owner=models.ForeignKey(User, on_delete=models.CASCADE, null=True)
    accessible=models.BooleanField(default=True)
    accessibilty_change_date=models.DateField(null=True)
    last_change_date=models.DateField(null=True)
    parent_dir=models.ForeignKey("self", on_delete=models.CASCADE, null=True)
    class Meta:
        unique_together = ('parent_dir', 'name', 'owner')
    def __str__(self):
        owner_name = None
        if self.owner is not None:
            owner_name = self.owner.username
        else:
            owner_name = "no one"
        return "Folder " + self.name + " of " + owner_name


class File(models.Model):
    name=models.CharField(max_length=100)
    description=models.TextField(blank=True)
    creation_date=models.DateField(default=timezone.now)
    owner=models.ForeignKey(User, on_delete=models.CASCADE, null=True)
    accessible=models.BooleanField(default=True)
    accessibilty_change_date=models.DateField(null=True)
    last_change_date=models.DateField(null=True)
    parent_dir=models.ForeignKey(Folder, on_delete=models.CASCADE, null=True)
    content=models.TextField()
    class Meta:
        unique_together = ('parent_dir', 'name', 'owner')
    def __str__(self):
        owner_name = None
        if self.owner is not None:
            owner_name = self.owner.username
        else:
            owner_name = "no one"
        return "File " + self.name + " of " + owner_name


class Section(models.Model):
    class Type(models.IntegerChoices):
        PROCEDURE = 0, _("Procedure")
        COMMENT = 1, _("Comment")
        DIRECTIVE = 2, _("Directive")
        VARIABLE = 3, _("Variable")
        ASSEMBLER = 4, _("Assemlber")
    
    class Status(models.IntegerChoices):
        CORRECT = 0, _("Correct")
        WARNING = 1, _("Warning")
        ERROR = 2, _("Error")
    
    name=models.CharField(max_length=200, blank=True)
    description=models.TextField(blank=True)
    creation_date=models.DateField()
    start_char=models.IntegerField()
    end_char=models.IntegerField()
    section_type=models.IntegerField(choices=Type.choices)
    status=models.IntegerField(choices=Status.choices)
    status_data=models.TextField(blank = True)

    file=models.ForeignKey(File, on_delete=models.CASCADE)
