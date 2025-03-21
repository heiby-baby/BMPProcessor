# Обработчик BMP-изображений

Это простой обработчик BMP-изображений, написанный на C++. Программа позволяет читать, отображать и изменять BMP-изображения, рисуя на них линии. Поддерживаются 24-битные и 32-битные BMP-файлы.

## Возможности

- **Чтение BMP-файлов**: Программа может читать BMP-файлы и извлекать данные о пикселях.
- **Отображение BMP-изображений**: Программа может отображать BMP-изображение в консоли с использованием ASCII-символов.
- **Рисование линий**: Программа может рисовать линии на изображении с использованием алгоритма **Брезенхэма**.
- **Сохранение BMP-файлов**: Изменённое изображение можно сохранить в новый файл.

## Требования

- **Компилятор C++**: Программа требует компилятор C++ с поддержкой стандарта C++11 или новее.
- **Windows**: Программа разработана для работы на Windows из-за использования специфичных для Windows заголовков (`Windows.h`).

## Использование

1. **Скомпилируйте программу**: Используйте компилятор C++ для компиляции исходного кода.
   ```powershell
   g++ main.cpp -o BMPProcessor.exe
   ./BMPProcessor.exe
