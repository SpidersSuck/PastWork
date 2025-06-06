# Generated by Django 5.2 on 2025-04-06 11:10

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('crimoapp', '0002_alter_disaster_disaster_type'),
    ]

    operations = [
        migrations.CreateModel(
            name='EmergencyAlert',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('latitude', models.FloatField()),
                ('longitude', models.FloatField()),
                ('timestamp', models.DateTimeField(auto_now_add=True)),
            ],
        ),
    ]
