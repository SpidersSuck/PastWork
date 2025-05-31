# Button Test Case
from django.test import TestCase, Client
from django.urls import reverse
from ..models import EmergencyAlert
import json


class EmergencyAlertTests(TestCase):
    def setUp(self):
        self.client = Client()
        self.url = '/app/alertsend/'

    def test_post_valid_alert(self):
        payload = {
            'lat': 46.745324,
            'lon': -117.153013
        }
        response = self.client.post(
            self.url,
            data = json.dumps(payload),
            content_type = 'application/json'
        )

        self.assertEqual(response.status_code, 200)
        self.assertEqual(EmergencyAlert.objects.count(), 1)
        self.assertEqual(response.json()["status"],"received")

    def test_post_invalid_data(self):
        response = self.client.post(
            self.url,
            data = json.dumps({}),
            content_type = 'application/json'
        )

        self.assertEqual(response.status_code, 400)
        self.assertEqual(EmergencyAlert.objects.count(),0)

    def test_post_request_not_allowed(self):
        response = self.client.get(self.url)
        self.assertEqual(response.status_code, 400)
        self.assertEqual(response.json()['error'], 'Invalid request')
