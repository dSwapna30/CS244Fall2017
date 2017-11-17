# Hooking up LIS3DH to ESP8266
- CS: Any GPIO pin (We used 5 for this assignment)
- SDO: 12
- SCL: 14
- SDA/SDI: 13
- GND: Any Ground pin
- VCC: 3.3V pin

# Setting up the nginx server and php-fpm
1. Start nginx with `sudo service nginx start`
2. Edit nginx configuration code with `sudo vim /etc/nginx/sites-available/default`
3. Start php-fpm with `sudo /usr/sbin/php5-fpm start`
4. View errors of nginx with `sudo less /var/log/nginx/error.log`
