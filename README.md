# Atlas
## Description
Библиотека генерирует атлас на основе графических данных прямоугольного вида.
## Instruction
### Создание пустого атласа
```C
HATLAS atlCreate(unsigned int uiWidth,unsigned int uiHeight,unsigned int uiBytePerPixel,unsigned int dx,unsigned int dy);
// uiWidth         - ширина атласа в пикселях.
// uiHeight        - высота атласа в пикселях.
// uiBytePerPixel  - количество байт на пиксель.
// dx              - количество пустого пространства в пикселях, которым обрамляется вставляемое изображение вдоль оси абсцисс.
// dy              - количество пустого пространства в пикселях, которым обрамляется вставляемое изображение вдоль оси ординат.
// Возвращаемое значение - обработчик атласа. В случае ошибки возвращается 0.
// В случае успешного выполнения функции, пустой атлас заполняется прозрачным белым цветом.
```
### Добавление изображения в атлас
```C
RECTUI *atlAdd(HATLAS hAtlas,void *pByte,unsigned int uiWidth,unsigned int uiHeight,const char *pName);
// hAtlas         - обработчик атласа.
// pByte          - вставляемое изображение в виде массива байт.
// uiWidth        - ширина вставляемого изображения в пикселях.
// uiHeight       - высота вставляемого изображения в пикселях.
// pName          - имя изображения (id изображения).
// Возвращаемое значение - указатель на прямоугольник, содержащий информацию о местоположении и размере вставленного изображения.
//                         В случае ошибки (изображение не было вставлено), возвращает 0.
```
Функция вызывается до тех пор, пока атлас не будет заполнен либо пока не будут упакованы все изображения.
### Получение готового атласа
```C
void *atlGetData(HATLAS hAtlas);
// hAtlas         - обработчик атласа.
// Возвращаемое значение - массив байт атласа.
```
### Завершение работы
```C
void atlDestroy(HATLAS hAtlas);
// hAtlas - обработчик атласа.
```
## Рекомендации
Для нормальной генерации атласа желательно отсортировать массив входящих изображений по площади в порядке убывания.
## AtlasMaker
<p><a href="https://sourceforge.net/projects/atlasmaker/files/AtlasMaker.zip/download">Ссылка</a> на Win32-приложение использущее библиотеку Atlas для генерации атласов. На вход подаётся psd-файл с непустыми растровыми слоями, на выходе получаем атлас или атласы (если все слои не помещаются в один атлас), подробный xml-файл с описанием, а также, если необходимо, лог работы программы.</p>
<p>P.S. В этой версии программы канал цвета в psd-файле должен быть восьмибитным.</p>
<p>P.P.S. <a href="https://github.com/Etwass/AtlasMaker">Исходный код AtlasMaker'а</a>.</p>

## License
GPL-3.0 License
