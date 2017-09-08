 Описание использования системы Menu в движке RCube


Инициализация меню

Описание текста используемого в меню в объекте - Button
    

struct SENTENCE_INIT_DATA
    {
     XMFLOAT4    Colour;      // цвет предложения RGBA
        float    ScrollSpeed; // Скорость сдвига предложения

          int    MaxLength;   // максимальная длинна предложения
          int    FontType;    // номер шрифта из созданных пользователем ( индекс шрифта в массиве векторов )
          int    ShowType;    // как именно отрисовывается, анимируется на экране ( разные шейдеры и функции )
          int    HideType;    // как именно убирается с экрана при установке render = false ( разные шейдеры и функции )
          int    PosX;        // позиция X на экране
          int    PosY;        // позиция Y на экране
          int    Level;       // 0 - текст в игре; 1 - HUD; 2 - Menu; 3 - Menu 1   и т.д.
         bool    Render;      // рисовать ли предложение на экране. False = исключается из списка в массиве отрисовываемых

    };



Инициализация меню :

    MainMenu = new MenuControrerClass;
        
MainMenu->Init ( hwnd, m_D3D->D3DGC, 
WindowPosX, WindowPosY,
        Buttons,  // Buttons,  Checkboxes, Labals, Edit
        5,        // Количество элементов типа Buttons
        NULL,     // ScrollBars
        0,        // Количество элементов типа ScrollBars
        NULL,     // Указатель но созданный и проинициализированный объект KF3dTextureAnimation , если в меню есть анимация фона ( NULL - Нет анимации фона )
                  // Анимация будет натянута на всё тело меню вместе с текстурой фона ( если фона нет, то будет одна анимация без фона поверх анимации )
        { 112.0f, 84.0f, 800.0f, 600.0f }, // Фактически размеры меню и позиция на экране  X,Y - позиция по X и Y; Z - Width, W - Height 
        MyMeneger->ShaderResourseArr[0],   // Указатель на текстуру фона
        MyMeneger->BlobsArr[5],            // Блоб от шейдера которым рисуем фон меню
        m_Text                             // Указатель на класс текста в движке, для создания строк в Label, Edit и вообще в Button
);

 Buttons - массив типа struct KFButton_Elements

    struct KFButton_Elements {
        float PosX;     // Коррдината на экране по X в пикселях
        float PosY;     // Коррдината на экране по Y в пикселях
        char *Label;    // начальный текст или NULL если не нужна строка текста привязанная из TextClass к Объекту Button
        int  Type;        // BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3
        int  EditType;    // ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3   если равно NULL или отличное от указанного, то кнопка не будет работать как Edit
        int  SentanceIndex; // Номер предложения из класса TextClass ( обязательно только для Label и Edit  для остальных можно -1 )
     struct SENTENCE_INIT_DATA* Data; // Описание строки текста из TextClass привязанной к объекту Button
        bool Checked;    // Для CheckBox - в нажатом ли состоянии кнопка при старте компонента
        bool Enabled;    // Доступен ли объект или рисуется текстура недоступности и объект не реагирует на мышь и прочее
// Label
        bool WaitingForKeyPress;    // Для Label который используется для привязки виртуальных кнопок в игре ( если False, то Label просто как строка текста - не реагирует на нажатие и пролёты мышки и прочее )
        int  VirtualKeyIndex;        // Привязка Label к виртуальной кнопке в движке. ( Для переопределения кнопки в меню )
// Label
        ID3D11Resource * OsnTextureResourse;  // Основная текстура которая рисуется когда элемент виден
        ID3D11ShaderResourceView * OsnTexture;// Основная текстура которая рисуется когда элемент виден
        ID3D11ShaderResourceView * IsClickTexture; // Текстура которая рисуется когда объект нажат мышкой
        ID3D11ShaderResourceView * IsMouseOnButtonTexture; // Текстура которая рисуется когда над объектом находится мышка
        ID3D11ShaderResourceView * IsNotEnalbledTexture;  // Текстура которая рисуется когда объект недоступен, но отображается
        ID3D10Blob * Blob;  // Нужен для создания Layout 
    };

  Структура описывающая строку текста ( в том числе привязанную к Button )
  struct SENTENCE_INIT_DATA
  {
   XMFLOAT4 Colour;   // цвет предложения RGBA
    float ScrollSpeed; // Скорость сдвига предложения

      int MaxLength;// максимальная длинна предложения
      int FontType; // номер шрифта из созданных пользователем ( индекс шрифта в массиве векторов )
      int ShowType; // как именно отрисовывается, анимируется на экране ( разные шейдеры и функции )
      int HideType; // как именно убирается с экрана при установке render = false ( разные шейдеры и функции )
      int PosX;     // позиция X на экране в пиклелях от левого верхнего угла.
      int PosY;     // позиция Y на экране в пиклелях от левого верхнего угла.
      int Level;    // 0 - текст в игре; 1 - HUD; 2 - Menu1... и т.д.
     bool Render;   // рисовать ли предложение на экране. False = исключается из списка в массиве отрисовываемых

  };


КАК СОЗДАТЬ Button

  KFButton_Elements Buttons[1];
  Buttons[0].PosX = 10.0f;  // Коррдината на экране по X в пикселях
  Buttons[0].PosY = 10.0f;  // Коррдината на экране по Y в пикселях
  Buttons[0].OsnTextureResourse = MyMeneger->ResourseArr[1]; 
  Buttons[0].OsnTexture = MyMeneger->ShaderResourseArr[1];
  Buttons[0].IsClickTexture = MyMeneger->ShaderResourseArr[7];
  Buttons[0].IsMouseOnButtonTexture = MyMeneger->ShaderResourseArr[6];
  Buttons[0].IsNotEnalbledTexture = MyMeneger->ShaderResourseArr[7];
  Buttons[0].Data = NULL; // Нет строки текста и соотвественно Data структура NULL - не заполнена 
  Buttons[0].Label = "";  // Начальный текст для инициализации строки из класса TextClass отсутствуюет
  Buttons[0].Type = BUTTON; // Создаётся объект BUTTON
  Buttons[0].EditType = NULL; // не имеет значения
  Buttons[0].Checked = false; // создаваемый объект BUTTON - НЕ будет работать как CHECKBOX  ( Начальная установка для CHECKBOX в нажатое состояние )
                              // Не имеет значения, если   Buttons[0].Type = НЕ РАВНО CHECKBOX
  Buttons[0].Enabled = true;  // Объект доступен для манипуляций с ним ( если true, то при манипуляциях мышки с этим объектом будут меняться текстуры для отображения на теле объекта )
  Buttons[0].WaitingForKeyPress = false; // работает для только для Button созданного как LABEL ( используется для переопределения кнопок в игре )
                                         // Не имеет значения, если   Buttons[0].Type = НЕ РАВНО LABEL 
  Buttons[0].Blob = MyMeneger->BlobsArr[5];  


  КАК СОЗДАТЬ CheckBox

  KFButton_Elements Buttons[1];
  Buttons[0].PosX = 10.0f;  // Коррдината на экране по X в пикселях
  Buttons[0].PosY = 10.0f;  // Коррдината на экране по Y в пикселях
  Buttons[0].OsnTextureResourse = MyMeneger->ResourseArr[1]; 
  Buttons[0].OsnTexture = MyMeneger->ShaderResourseArr[1];
  Buttons[0].IsClickTexture = MyMeneger->ShaderResourseArr[7];
  Buttons[0].IsMouseOnButtonTexture = MyMeneger->ShaderResourseArr[6];
  Buttons[0].IsNotEnalbledTexture = MyMeneger->ShaderResourseArr[7];
  Buttons[0].Data = NULL; // Нет строки текста и соотвественно Data структура NULL - не заполнена 
  Buttons[0].Label = "";  // Начальный текст для инициализации строки из класса TextClass отсутствуюет
  Buttons[0].Type = CHECKBOX; // Создаётся объект BUTTON типа CHECKBOX с фиксацией нажатий
  Buttons[0].EditType = NULL; // создаваемый объект BUTTON - НЕ будет работать как EDIT
  Buttons[0].Checked = false; // ( Начальная установка для CHECKBOX в нажатое состояние )
  Buttons[0].Enabled = true;  // Объект доступен для манипуляций с ним ( если true, то при манипуляциях мышки с этим объектом будут меняться текстуры для отображения на теле объекта )
  Buttons[0].WaitingForKeyPress = false; // работает для только для Button созданного как LABEL ( используется для переопределения кнопок в игре )
  Buttons[0].Blob = MyMeneger->BlobsArr[5];


  КАК СОЗДАТЬ LABEL

  SENTENCE_INIT_DATA *Data5;
  Data5 = new SENTENCE_INIT_DATA;
  ZeroMemory( Data5, sizeof( SENTENCE_INIT_DATA ) );

  Data5->MaxLength = 20;
  Data5->PosX = 10;
  Data5->PosY = 330;
  Data5->ShowType = 1;
  Data5->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
  Data5->Render = true;
  Data5->FontType = 0;
  Data5->Level = 2;

  KFButton_Elements Buttons[1];
  Buttons[0].PosX = 10.0f;  // Коррдината на экране по X в пикселях
  Buttons[0].PosY = 10.0f;  // Коррдината на экране по Y в пикселях
  Buttons[0].OsnTextureResourse = MyMeneger->ResourseArr[1]; 
  Buttons[0].OsnTexture = MyMeneger->ShaderResourseArr[1];
  Buttons[0].IsClickTexture = MyMeneger->ShaderResourseArr[7];
  Buttons[0].IsMouseOnButtonTexture = MyMeneger->ShaderResourseArr[6];
  Buttons[0].IsNotEnalbledTexture = MyMeneger->ShaderResourseArr[7];
  Buttons[0].Data = Data5; // Для LABEL обязательно нужно заполнить этот массив
  
  Buttons[0].Label = "Label"; // Начальный текст для инициализации строки из класса TextClass. Эта страка может быть пустой. 
                              // Строка текста в TextClass создаётся размером Data5->MaxLength = 20; и не будет иметь начального текста
                              // если Buttons[0].Label = "";

  Buttons[0].Type = LABEL; // Создаётся объект BUTTON типа LABEL
  Buttons[0].EditType = NULL; // не имеет значения
  Buttons[0].Checked = false; // не имеет значения
  Buttons[0].Enabled = true;  // Объект доступен для манипуляций с ним ( если true, то при манипуляциях мышки с этим объектом будут меняться текстуры для отображения на теле объекта )
  Buttons[0].WaitingForKeyPress = true; // ( если true, то при нажатии на объект мышкой используется для переопределения кнопок в игре. Если false, то Label просто отображает текст как обычный Label )
  Buttons[0].Blob = MyMeneger->BlobsArr[5];

  не забываем после вызова Init для меню удалить
    delete [] Data5;


  КАК СОЗДАТЬ EDIT

  SENTENCE_INIT_DATA *Data4;
  Data4 = new SENTENCE_INIT_DATA;
  ZeroMemory( Data5, sizeof( SENTENCE_INIT_DATA ) );

  Data4->MaxLength = 11;
  Data4->PosX = 10;
  Data4->PosY = 330;
  Data4->ShowType = 1;
  Data4->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
  Data4->Render = true;
  Data4->FontType = 0;
  Data4->Level = 2;


  Buttons[4].PosX = 10.0f;
  Buttons[4].PosY = 330.0f;
  Buttons[4].OsnTextureResourse = MyMeneger->ResourseArr[11];
  Buttons[4].OsnTexture = MyMeneger->ShaderResourseArr[11];
  Buttons[4].IsClickTexture = NULL;
  Buttons[4].IsMouseOnButtonTexture = NULL;
  Buttons[4].IsNotEnalbledTexture = NULL;
  Buttons[4].Data = Data4; 
  Buttons[4].Label = "Мой текст"; // Начальный текст для инициализации строки из класса TextClass. Эта страка может быть пустой. 
                                  // Строка текста в TextClass создаётся размером Data5->MaxLength = 20; 
                                  // и имеет начальный текст Buttons[4].Label = "Мой текст";

  Buttons[4].Type = EDIT; // Создаётся объект BUTTON типа Edit
  Buttons[4].EditType = ANYSYMBOL; // 1- ANYSYMBOL ; 2 - AS_INTEGER ; 3 - AS_FLOAT   ( 0 - NULL и людое иное значение аннулируют работу объекак как Edit ) 
                                   // ( ввод в поле Edit будет воспринимать только символы допустимые для соответствующего типа данных)

  Buttons[4].Checked = false; // не имеет значения
  Buttons[4].Enabled = true;  // Объект доступен для манипуляций с ним ( если true, то при манипуляциях мышки с этим объектом будут меняться текстуры для отображения на теле объекта )
  Buttons[4].WaitingForKeyPress = false; // не имеет значения
  Buttons[4].Blob = MyMeneger->BlobsArr[5];


    не забываем после вызова Init для меню удалить
    delete [] Data4;