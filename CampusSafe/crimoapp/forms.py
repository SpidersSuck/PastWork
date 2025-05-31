from django import forms
from django.contrib.auth.models import User
from django.contrib.auth.forms import UserCreationForm

class UserRegistrationForm(UserCreationForm):
    username = forms.CharField(
        max_length=8,
        required=True,
        help_text="Enter an 8-digit numeric username.",
        widget=forms.TextInput(attrs={'placeholder': '8-digit username'}),
    )

    password1 = forms.CharField(
        label="Password",
        widget=forms.PasswordInput(attrs={'placeholder': 'Password'}),
        help_text="Enter a strong password.",
    )

    class Meta:
        model = User
        fields = ['username', 'password1']

    def clean_username(self):
        username = self.cleaned_data.get('username')
        if not username.isdigit() or len(username) != 8:
            raise forms.ValidationError("Username must be an 8-digit number.")
        if User.objects.filter(username=username).exists():
            raise forms.ValidationError("Username is already taken.")
        return username