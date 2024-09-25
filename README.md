# RinexReader

RINEX (Receiver Independent Exchange Format) — формат обмена данными для файлов исходных данных спутниковых навигационных приёмников. Он позволяет пользователям производить пост-обработку полученных данных для выполнения более точных вычислений — обычно с помощью других данных, неизвестных приемнику.

Данная программа читает файлы:
- файл измерений (расширение .YYO, где YY-номер года, а O - observation, например .19О, или .obs)
- файлы эфемерид в зависимости от используемых систем

<details><summary>Типы файлов эфемерид и их расширения</summary>

- .YYN – GPS,
- .YYG – ГЛОНАСС,  
- .YYC – BeiDou 
- .YYL – Galileo  
- .YYJ – QZSS  
- .YYI – IRNSS  
- .nav – смешанные эфемериды
</details>

Поддержка чтения только только RINEX v3.* .

А для файлов эфемерид : GPS, GLONASS, GALILEO, BEIDOU.

## Installation 

Откройте файл проект *.pro в Qt Creator. В будуще можно будет загрузить как библиотеку.

## Possibilities and work 

Для начала нам нужно созадть экземляр нашего ридера.
При объявлении мы можем сразу указать пути к нишему RINEX файлу(obs - измерения, nav - вектор путей для эфемерид ).

```
RinexReader rr(obs,navPaths);
```

Так же можно можно читать разные типы раздельно, для этого нужно указать путь и тип файла.

```
RinexReader rr(pathObs, RinexType::OBSERVATION);
RinexReader rr_only_nav(navGPS, RinexType::NAVIGATION); 
RinexReader rr_only_nav_(navPaths, RinexType::NAVIGATION);
```

Если при чтении списка только навигационных файлов тип можно не указывать, он сам поймет, что это nav файлы.

```
RinexReader rr_only_nav_(navPaths);
```

## Getting data from a file

### Obs файлы

Во время инициализации идет проверка типа файла(GPS , Glonass, Mixed и т.д.) и его версии. Далее если версия или тип не поддреживаются файл ридер не будет читать файл.

Если все хорошо, то мы сможем полуить данные из файла.

Получение информацию заголовка :

```
Rinex3Obs::ObsHeaderInfo headObs = rr.getObsHeaderInfo();
```

В даннщм случае из `headObs` мы можем получить практические любые данные из хэдера. Про названия полей и за что они отвечают смотреть [тут](http://acc.igs.org/misc/rinex304.pdf).


Получение списка всех эпох:

```
QList<Rinex3Obs::ObsEpochInfo> inf = rr.getEpochs();
```

Если файл большой это довольно долгая и затратная операция поэтому при первом ее вызове ее результат сохранится, а в дальнейшем при повторном вызове этой функции будет выдаться ссылка на этот лист. Он сохранится либо до переопределения пути в obs файлу , вызова деструктора или вы можете сами отчисить obs data через метод `clearObs()`.

Но будьте осторожны т.к. `clearObs()` обнулит все данные связаные с obs файлом, включая путь к нему.

Далее немного о сруктуре этого листа:

` QList<Rinex3Obs::ObsEpochInfo> `

-   epochRecord; - вектор даты и времени
-   recClockOffset; 
-   gpsTime; 
-   numSatsGPS; - кол-во зависей для системы GPS
-   numSatsGLO;
-   numSatsGAL;
-   numSatsBEI;
-   observations; - map отсортерованныей по типу системы --> номер спутника - данные `std::map<std::string, std::map<int, std::vector<double>>>`

### Nav файлы

После инициализации мы  можем прчитать файлы эфемерид один за други , по индексу или пути который уже добавлен(можно добавить еще) если файлов несколько, для одного наналогично.

```
ViewNav viewNav;
rr.nextNav();
viewNav= rr.getNav();
rr.nextNav();
viewNav= rr.getNav();
rr.nextNav();
viewNav = rr.getNav();
rr.clearNavData();
rr.nextNav();
viewNav = rr.getNav();
```

or 

```
rr.readNav(0); // по индексу как расположен в векторе
rr.readNav(navGPS); // по пути который уже добавлен
```

`ViewNav` - структура для предаствления nav файлов и работы тоько м полями.

### CSV 

Так же вы можете созранить ваши прочитанные данные в csv формат( вы можете явно задать разделител после указания типа, по-умолчанию это "," , что является стандартом в csv файлах):

```
QString pathCSVobs = "C:/Utils/RinexSample/infEpochs.csv";
QString pathCSVnav = "C:/Utils/RinexSample/infNavs.csv";
rr.saveAsCSV(pathCSVobs, RinexType::OBSERVATION);
rr.saveAsCSV(pathCSVnav, RinexType::NAVIGATION);
```


## Impotant notes

Вам доступно 3 вида метода clean() :
- `clearObs()` - очищает все данные связанные с obs файлом
- `clearRRNav()` - очищает все данные связанные с nav файлами
- `clearNavData()` - очищает только данные прочитанного файла(очищает информацию о прочитанных nav данных), но сохраняет пути к файлам для дальнейшем работы с ними.

## Generate names

Был добавлен специальный класс для генерации url адреса и имен файлов для скачивани их с сервера(пок немного сарой еще):

Примеры есть в коде(продублирую)

```
RinexNameGenerator rng;
std::string extension = "rnx.zip";
QDateTime::currentSecsSinceEpoch();// new
QDateTime::currentDateTimeUtc().toTime_t();// (obsolete) ver. 5.15 and below
std::cout <<  rng.generateUrl(IdPDP::BARA, "12", "24", "1", "2024", extension) << std::endl;
std::cout <<  rng.generateUrl(IdPDP::BARA, 12, 24, 1, 2024, extension) << std::endl;
std::cout <<  rng.generateUrl(IdPDP::KLEC, "24/1/2023 12:00:00", extension) << std::endl;
std::cout <<  rng.generateUrl(IdPDP::KLEC, QDateTime::currentSecsSinceEpoch(), extension) << std::endl;
std::cout <<  rng.generateUrl(IdPDP::BARA,QDateTime::currentSecsSinceEpoch(), extension) << std::endl;
out << "File names: "<< endl;
for( const std::string& x : rng.generateRinexFilesNames(IdPDP::BARA,QDateTime::currentSecsSinceEpoch()))
    std::cout << x << std::endl;
```
