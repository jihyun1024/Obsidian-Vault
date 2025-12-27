FROM php:8.1-apache

RUN apt-get update && apt-get install -y \
    libzip-dev \
    zip \
    unzip \
    && docker-php-ext-install zip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN a2enmod rewrite
RUN a2enmod headers

WORKDIR /var/www/html

COPY deploy/ /var/www/html/

RUN chown -R www-data:www-data /var/www/html
RUN chmod -R 755 /var/www/html
RUN chmod -R 777 /var/www/html/uploads

RUN echo "file_uploads = On" >> /usr/local/etc/php/conf.d/uploads.ini
RUN echo "upload_max_filesize = 5M" >> /usr/local/etc/php/conf.d/uploads.ini
RUN echo "post_max_size = 5M" >> /usr/local/etc/php/conf.d/uploads.ini
RUN echo "max_file_uploads = 20" >> /usr/local/etc/php/conf.d/uploads.ini

RUN echo "<Directory /var/www/html>" >> /etc/apache2/sites-available/000-default.conf
RUN echo "    AllowOverride All" >> /etc/apache2/sites-available/000-default.conf
RUN echo "</Directory>" >> /etc/apache2/sites-available/000-default.conf

EXPOSE 8000

RUN sed -i 's/Listen 80/Listen 8000/' /etc/apache2/ports.conf
RUN sed -i 's/:80>/:8000>/' /etc/apache2/sites-available/000-default.conf

CMD ["apache2-foreground"]
