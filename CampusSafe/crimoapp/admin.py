
# Register your models here.
from django.contrib import admin
from .models import Disaster, EmergencyAlert

admin.site.register(Disaster)
admin.site.register(EmergencyAlert)

