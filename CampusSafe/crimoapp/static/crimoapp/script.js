const button = document.getElementById('emergency-button');
        const message = document.getElementById('alert-message');
        let countdown = 10;
        let countdownInterval;

        button.addEventListener('mousedown', () => {
            button.innerText = `Hold ${countdown}`;
            message.innerText = '';
            countdownInterval = setInterval(() => {
                countdown--;
                if (countdown > 0) {
                    button.innerText = `Hold ${countdown}`;
                } else {
                    clearInterval(countdownInterval);
                    button.innerText = '📍';
                    triggerEmergency();
                }
            }, 1000);
        });

        button.addEventListener('mouseup', () => {
            clearInterval(countdownInterval);
            if (countdown > 0) {
                message.innerText = "Alert canceled button not held long enough.";
                button.innerText = 'Alert';
                countdown = 10;
            }
        });

        function triggerEmergency() {
            message.innerText = "Help is on the way! Sending your location!";
            navigator.geolocation.getCurrentPosition(function(position) {
                const lat = position.coords.latitude;
                const lon = position.coords.longitude;

                fetch('/crimoapp/alertsend/', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ lat, lon })
                })
                .then(response => response.json())
                .then(data => {
                    console.log('Server response:', data);
                })
                .catch(err => {
                    console.log('Error:', err);
                });
            });
        }