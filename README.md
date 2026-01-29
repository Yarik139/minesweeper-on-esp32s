# minesweeper-on-esp32s/in Russian
Итак, что нам понадобится: esp32 (у меня версия S), сенсорный дисплей на ILI9341, dupont провода. При желании можно всё соединить на макетной плате.
![photo_5465171654976671512_y](https://github.com/user-attachments/assets/cb27809f-9eb3-448d-88d5-7290d71e6a7e)
![photo_5465171654976671513_y](https://github.com/user-attachments/assets/22d66409-4968-43be-9b72-e4aebaad8d0b)
![photo_5465171654976671511_y](https://github.com/user-attachments/assets/a3e21634-4dae-4e61-a37e-a6d37439f706)
Соединяем всё по схеме:
<img width="800" height="1280" alt="подключение" src="https://github.com/user-attachments/assets/5066cc2b-6e4d-436b-99b6-b2d0899aab17" />
Далее скачиваем Arduino ide с официального сайта (в России к сожалению не доступен, но думаю вы с этим справитесь). Далее переходим в Arduino ide:
Tools->Board->Boards Manager-> ищем esp32 by Espressif Systems и скачиваем.
Перейдём к скачиванию библиотек: Sketch->Include Library->Manage Libraries. Скачиваем библиотеки: Adafruit GFX Library, Adafruit BusIO, Adafruit ILI 9341, XPT2046_Touchscreen.
Теперь нужно написать сам код. Дисплей по умолчанию не откалиброван, поэтому нужно будет сделать это в коде. Сначала откройте в Arduino ide прикриплённый к проекту код (minesweeper.cpp). Затем открываем монитор порта (ctrl+shift+m). Выбираем скорость 115200 baud. Теперь возьмите стилус (или любой тонкий предмет) и нажмите в верхний левый (MIN значения) и в нижний правый угол экрана (MAX значения) (нажимайте быстро, один раз). В мониторе должны появиться 2 строки типа Raw X: ... | Raw Y: ... Эти 4 числа мы занесём в код в строк:
// НАСТРОЙКИ КАЛИБРОВКИ (Вставьте свои числа)
#define TS_MINX ...   // Сюда пишем Raw X из левого верхнего угла
#define TS_MAXX ...  // Сюда пишем Raw X из правого нижнего угла
#define TS_MINY ...   // Сюда пишем Raw Y из левого верхнего угла
#define TS_MAXY ...  // Сюда пишем Raw Y из правого нижнего угла
Настройте калибровку и запишите код на плату.
