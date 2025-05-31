from django.urls import path
from crimoapp import views

urlpatterns = [
    path('', views.home, name='home'),
    path('resources/', views.resources, name='resources'),
    path('dashboard/', views.dashboard, name='dashboard'),
    path('report/', views.report, name='report'),
    path('registration/', views.register_user, name='registration'),
    path('registration_success/', views.registration_success, name='registration_success'),
    path('login/', views.login_user, name='login'),
    path('logout/', views.logout_user, name='logout'),
    path('report_history/', views.view_report_history, name='view_report_history'),
    path('incident-map/', views.incidentMap, name='incidentMap'),
    path('alertsend/', views.alertsend, name='alertsend')
]
