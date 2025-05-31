# models.py
from django.db import models
from django.db import models

class EmergencyAlert(models.Model):
    latitude = models.FloatField()
    longitude = models.FloatField()
    timestamp = models.DateTimeField(auto_now_add=True)

class Disaster(models.Model):
    DISASTER_TYPES = [
        ('Flood', 'Flood'),
        ('Earthquake', 'Earthquake'),
        ('Cyclone', 'Cyclone'),
        ('Landslide', 'Landslide'),
        ('Tsunami', 'Tsunami'),
        ('Storm','Storm'),
        ('Drought','Drought'),
        ('Epidemic','Epidemic')
    ]

    disaster_type = models.CharField(max_length=50, choices=DISASTER_TYPES)
    location = models.CharField(max_length=100)
    data = models.TextField()
    

    def __str__(self):
        return f"{self.disaster_type} in {self.location}"


