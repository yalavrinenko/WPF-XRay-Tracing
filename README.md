# xray-tracing

#### Версия от 26.08.2018
1. Последний релиз с новым интерфейсом: не работает Trace, т.е. геометрия схемы считается правильно, 
далее трейсинг отображается как Done, без выдачи ошибок, но при этом по всем линиям показывается NaN под числом отраженных фотонов.
графики тоже ничего не показывают. При запуске трейса с галкой про Gen.wave.sequence - идет ошибка и закрытие программы.
**Добавлен дистрибутив VS2017 для установки дополнительных зависимостей. Файл vc_redist.x64.exe**

2. Предыдущий пункт наталкивает на мысль, что в новой сборке нужна какая-то замена отсутствующему терминальному окну 
(чтобы была возможность отслеживать ошибки). Может быть предусмотреть дополнение интерфейса таким полем "terminal view"?
**Добавлено окно с stdout. Кнопка show tracing output в окне расчета.**

3. Много народу сейчас пользуется старым релизом, и шлют свои вопросы, которые могут быть актуальны для модификации как старой так и новой версии. 
	**Старая версия не поддерживается. Рекомендуется переходить на новую**

	3.1. похоже, что линия из дополнительного порядка отражения сейчас не может быть 	использована в качестве reference line. (идет ошибка line is not in the base). **Исправлено в новой версии** 
	3.3. после поля Reference L у нас идет поле Scattered angle, в котором при этом 	отображается величина Bragg angle, надо поправить, чтобы выводилось значение 	Scattered angle. **Исправлено**
	3.4. само поле scattered angle сейчас выступает только как справочное. Нужно, чтобы 	при изменении в нем значения вручную, программа пересчитывала центральную 	длину волны и апдейтила схему. **В новой версии эта возможность реализована**
	3.5. размеры детектора (detector limits) cейчас выбираются минимальными, 	предлагаю задать по дефолту размер +-5 мм по вертикали и +-30 мм по горизонтали.  **Установлен размер детектора по умолчанию 60х10**
	3.6. Подписи на кнопках справа  про additional orders лучше заменить на "Include 	additional orders" , "Remove additional orders", a то люди путаются в функционале **Надписи изменены**
	3.7. В поле Wavelength limits координаты краев отражения лучше подписать как 	"Position Lmin, Position Lmax", выделение этого поля цветом - скорее не нужно. **Надписи изменены**
	3.7. В поле «Crystal MxS»: Cейчас размер телесного угла определяется квадратом со 	стороной, записанной в левое окошко (M), что не всегда верно. Можно ли сделать, 	
	чтобы бралось большее из значений, записанных в окно Crystal MxS. **Телесный угол определяется по максимальному размеру**

5. Можем ли мы доработать часть, связанную с поглощающим объектом? Насколько сейчас функциональны поля в разделе Object? 
Если функциональны, то в каком формате надо готовить файл, описывающий попиксельно пропускание различных областей объекта?
**Добавлен функционал для объекта. Объект задается в текстовом файле с разделителем "space" в виде матрицы MxN, в строках и столбцах которой записано число от 0 до 1 соответствующее вероятности прохождения луча через 
объект в данном месте. Пример:**

```
0 0 0 0 0 0 0 0 0 0
0 0 0 0 1 1 0 0 0 0
0 0 0 0 1 1 0 0 0 0
0 0 0 0 1 1 0 0 0 0
0 1 1 1 1 1 1 1 1 0
0 1 1 1 1 1 1 1 1 0
0 0 0 0 1 1 0 0 0 0
0 0 0 0 1 1 0 0 0 0
0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0
```



#### Версия от 24.09.2017

0. Т.о., в output файле хочется получить две формулы (полиномы), апроксимирующие значения Magnification и Efficiency. рассчитанные для каждой введенной в расчет линии, функцией от Wavelength, 
т.е. формулы вида Magnification = a0+a1*(Wavelength)+a2*(Wavelength)^2.
Для оптимизации счетного времени предлагаю предусмотреть в интерфейсе опцию флага "Gen.wave sequence", аналогичную флагу "Display all". 
Без флага запускается обычный счет по введенным линиям, апроксимационные зависимости строятся по имеющимся точкам, посчитанным для реальных линий. 
При установке флага в список выбранных линий добавляется последовательность из, скажем, 20 виртуальных линий, 
эквидистантно расположенных по спектру между Lmin и Lmax. 
Их можно нигде вообще не отображать, по крайней мере точно не выводить в Output файл, но использовать для повышения точности апроксимационных кривых. 
**Добавлен checkbox Gen.wave sequence. В output файл добавлена секция:**
```text
Additional fits
Efficiency curve:F(wavelenght=X) = 8.15708721617121E-06*X^2 + 8.15708721617121E-06*X + -2.40222421955762E-05
Magnification curve:F(wavelenght=X) = 0.00385*X^2 + -0.06693*X + 0.36531
***************************************************************
Name	Wavelength	Efficiency	Fit_Efficiency	Efficiency_fit_quality	Magnification	Fit_Magnification	Magnification_fit_quality
Al He He9	6.0134	2.83401737198593E-06	2.86725258592369E-06	0.01173	0.10279	0.1019	0.00862
Al He He8	6.0322	2.75765163441745E-06	2.88181473584517E-06	0.04502	0.10298	0.10151	0.01425
Al He He6	6.1028	2.90189802760236E-06	2.93263197971023E-06	0.01059	0.09226	0.10008	0.0848
Al He Hed	6.1754	3.08856983054754E-06	2.9785171111576E-06	0.03563	0.09679	0.09865	0.01918
Al He Heg	6.3137	3.07159966664344E-06	3.04805023765682E-06	0.00767	0.09361	0.09604	0.02596
Al He Heb	6.6343	3.0970549124996E-06	3.11906946052996E-06	0.00711	0.08927	0.09054	0.0143
```


#### Версия от 04.07.2017

0. Изменено API для написания графической оболчки. Совершен переход из Python GTK на WPF.NET. Это позволяет боле комфортно поддерживать 
код и добавлять в него новый функционал.

#### Версия от 20.06.2017

0. Сейчас в output файле значение Source solid angle чаще всего составляет единицы срад.
При этом реально апертура кристалла занимает менее 0.1, а скорее около 0.02 срад.
Ты уверен в получаюшемся числе?этом расчете)? 
Если да, то может быть телесный угол можно выбирать существенно меньшим и тем самым
уменьшать время счета?
**Исправлена ошибка вывода телесного угла в output файл**

1. Редактура содержания Output файла. 
Чего удалить, что исправить, и что добавить - см. прилагаемый файл. 
Основное - это расчет и вывод данных об эффективности отражения в каждой линии. 
См. предлагаемые формулы в соответствующих колонках файла. 
Нам прежде всего нужно значение эффективности всей системы, 
т.е. отношение зарегистрированных на детекторе лучей ко всем лучам, 
излученным в полный телесный угол 4Pi. 
**Формат output файла изменен. Таблица в программе оставлена без изменений.**

2. Выгрузка графических результатов расчета в файлы. 
По-видимому, нам не требуется выводить в файлы все получившиеся картинки, 
а только две - _изображение_  плоскости детектора и точки отражения в плоскости кристалла.
Но и то и другое надо выводить в tiff 16bit формате, 
а не в сжатых форматах. 
Размер пикселя для tiff картинки у нас задается в соответствующем поле. 
1 луч, попавший в пиксель - 1 отсчет, 
**Формат изображений изменен в соответствии с описаннием выше**

3. По умолчанию в качестве базы данных линий у нас выбирается файл trans-db.txt. 
Хорошо бы это имя файла отображать в соответсвующем поле 
(сейчас там пусто, если не выбирать новый файл в ручную).
**Проблема решается.**

4. в последнем релизе по прежнему отсутствует файл order1.par
Программа запускается и при первом запуске файл генерится, но тем не менее.
 **Наличие этого файла не влияет на функционал программы. Если есть случаи когда она не 
 работает без этого файла?**

5. после успешного завершения счета в терминале выдаются следующие ошибки 
(но пока не вижу, отражаются ли они на чем-нибудь)
('results\\Order_1.dump', 3705)
[['results\\Order_1\\117.151.dmp']]
('results\\Order_1\\117.151.dmp', 3702)
('results\\Order_1\\117.151.dmp', 3702)
C:\Serega\EXPERI~1\XRAY-T~2\XRAY-T~1\RAY-TR~1\numpy\core\_methods.py:59: RuntimeWarning: Mean of empty slice.
C:\Serega\EXPERI~1\XRAY-T~2\XRAY-T~1\RAY-TR~1\numpy\core\_methods.py:71: RuntimeWarning: invalid value encountered in double_scalars
C:\Serega\EXPERI~1\XRAY-T~2\XRAY-T~1\RAY-TR~1\numpy\core\_methods.py:83: RuntimeWarning: Degrees of freedom <= 0 for slice
C:\Serega\EXPERI~1\XRAY-T~2\XRAY-T~1\RAY-TR~1\numpy\core\_methods.py:95: RuntimeWarning: invalid value encountered in true_divide
C:\Serega\EXPERI~1\XRAY-T~2\XRAY-T~1\RAY-TR~1\numpy\core\_methods.py:117: RuntimeWarning: invalid value encountered in double_scalars
**Это предупреждения при вызове функций для пустых векторов. Влияния на работу программы не должны оказывать.**

И совсем отдельная задача, новый функционал, который хотелось бы реализовать (по времени срочности нет).
Если помнишь, у нас была такая кнопка Gen.waves, которая создавала массив виртуальных линий с некоторым шагом. Сама по себе она оказалсь не нужна и я попросил ее удалить. Но на самом деле что-то подобное нам нужно, для вычисления монотонных зависимостей эффективности отражения и линейного увеличения схемы от длины волны.

Т.о., в output файле хочется получить две формулы (полиномы), апроксимирующие значения Magnification и Efficiency. рассчитанные для каждой введенной в расчет линии, функцией от Wavelength, 
т.е. формулы вида Magnification = a0+a1*(Wavelength)+a2*(Wavelength)^2.
Для оптимизации счетного времени предлагаю предусмотреть в интерфейсе опцию флага "Gen.wave sequence", аналогичную флагу "Display all". Без флага запускается обычный счет по введенным линиям, апроксимационные зависимости строятся по имеющимся точкам, посчитанным для реальных линий. 
При установке флага в список выбранных линий добавляется последовательность из, скажем, 20 виртуальных линий, эквидистантно расположенных по спектру между Lmin и Lmax. Их можно нигде вообще не отображать, по крайней мере точно не выводить в Output файл, но использовать для повышения точности апроксимационных кривых. 
**В процессе**



#### Версия от 14.02.2017

1. Расчет угла поворота плоскости детектора - выдает совсем неверные
числа,  Это угол между направлением от центра детектора на точку в
фокусе кристалла и нормалью к поверхности детектора ( = перпендикуляр
к линии, соединяющей центр детектора и центр кривизны кристалла)
**Изменено в соответствии с требованием**

2. Выдача значения FRO angle - должно быть  = 180-FRO(current value).
И желательно выдавать его в log сразу, как и остальные параметры
(сейчас оно появляется только после trace)
**Добавлено выведение FRO в Ouput log до начала расчета**

3. Терминологически - ныненшняя величина Scattering angle на самом
деле является Bragg angle,  а Incident angle  = 90 - Bragg.  Надо
поправить и выдавать оба значения.
**Названия заменены на Bragg angle и Incident angle**

4. Из графика дисперсионной схемы следует, что лучи для разных длин
волн не пересекаются в меридиональной плоскости после отражения от
кристалла (между кристаллом и детектором). Фокусное расстояние в
меридиональной плоскости равно (R/2)*cosFi (!) - должны пересекаться! 
***Исправлено.**

5. 
Порядки отражения - в целом, работает так, как надо, пользоваться
удобно. НО - если задать в качестве Main order какой-то старший
порядок, а в качестве Addit. order - первый порядок, то этот 1 порядок
не отображается на схеме. Аналогично, при таких установках вообще не
генерятся additional lines.
Т.е. нужно, чтобы и в таблицу линий, и на график выводились линии во
всех порядках, указанных в двух окнах Main и Additional, и убирать
дубликаты. **Предположительно исправлено**
Кроме того, будет правильно ввести ограничение - генеривать для
порядков только те линии, получившаяся длина волны которых (табличная
\* порядок) не превышает максимального из введеных 2d кристалла (линии
с большей длиной волны не отразятся ни при каких условиях, можно не
показывать) **Линни длины больше 2d не генерируются по нажатию Addit. order. В ручном 
режиме их добавить можно.**

6. В таблице результатов трейсинга для каждой спектральной линии
столбец X-coord  - значения выдаются со знаком минус. Надо
инвертировать, чтобы X-coord по знаку соответствовала положению линий
на графике внизу. **Столбец X-coord инвертирован**

7. Поясни, плз, содержание колонки Captured [n]. Это число лучей с
данной длиной волны, попавших во всю апертуру кристалла? Или число
лучей, излученных в телесный угол Source solid angle? Если последнее,
то лучше переобозвать колонку как Emitted. **Captured - это число лучей с данной длиной 
волны, попавших во всю апертуру красталла. Число лучей излеченных в телесный 
 угол это Rays\*Intensity.** 

8. На графике - Selected lines, в режиме Display all желательно также
отображать Lmin и Lmax. **Добавлено отобрадение Lmin и Lmax  в режиме Display All**

9. Кнопка Generate waves - по-моему, можно удалить. Вводили ее для
отладки, не могу придумать, когда этот функционал может понадобиться
при реальных расчетах. **Убрана кнопка Generate waves**

10. Кнопки Del. additional lines и Clear all lines лучше поменять
местами, чтобы Clear all была нижней, логически последней в ряду.
**Порядок изменен**

11. Надписи на кнопках здесь же - надо все-таки выровнять, т.е. начало
надписей сделать с равным отступом от левой границы. В крайнем случае
просто сделать центрирование надписи по полю. **Надписи выровнены по центру**

12. При поиске линии в базе пробел считается содержательным а не
служебным символом. Лучше считать, что введенные в запрос символ
пробела соответстсвует логическому элементу AND - то есть должен
вестить поиск по записям, содержащим оба параметра. Например, запрос
вида "Al He" должен возвращать только линии Al с обозначением He либо
в названии линии, либо в поле изоэлектронной последовательности.
Сейчас же такой запрос вообще ничего не возвращает.**Исправлено. Пробел опять 
является содержательным символом. Операция OR представлена символом ";".**

13. Добавить в финальный релиз директорию Par
**Добавлено**

14. Можно ли переместить файл с базой линий в ту же директорию, что и
файлы с кривыми отражения (чтоб народ не лазил по служебным папкам, а
заходил только в одну)?
И можно ли переименовать эту папку из example в Input? Замечание не
принципиальное - если требует много усилий, можно проигнорировать.
**Переименовано. Файл с БД помещен в директорию Input. Добавлена возможность выбора
файла с базой данных. По умолчанию берется из Input/trans-db.txt**

15. Сделать отдельный депозитарий с финальным релизом, чтобы люди
могли скачать только его, а не весь проект.
**Сделано.**
