#include "atlas.h"

#ifndef _INC_STDIO
#include <stdio.h>
#endif  // _INC_STDIO

#ifndef _INC_STRING
#include <string.h>
#endif  // _INC_STRING

#ifndef _INC_MALLOC
#include <malloc.h>
#endif  // _INC_MALLOC

typedef struct tagNODE                                // Структура, описывающая дерево прямоугольников.
  {
    void *m_pLeft;                                    // Указатель на левый дочерний лист дерева.
    void *m_pRight;                                   // Указатель на правый дочерний лист дерева.
    RECTUI m_rctui;                                   // Прямоугольник, занимаемый картинкой в атласе.
    char *m_pName;                                    // Имя картинки.
  } NODE;
typedef struct tagATLAS                               // Структура, представляющая информацию об атласе.
  {
    unsigned int m_uiBytePerPixel;                    // Количество байт на пиксель.
                                                      // P.S. Предполагается, что все добавляемые текстуры будут иметь
                                                      //      именно такую глубину цвета.
    unsigned int m_uiWidth;                           // Ширина атласа в пикселях.
    unsigned int m_uiHeight;                          // Высота атласа в пикселях.
    unsigned int m_uiDX;                              // Отступ в пикселях вокруг добавляемых текстур вдоль оси абсцисс.
    unsigned int m_uiDY;                              // Отступ в пикселях вокруг добавляемых текстур вдоль оси ординат.
    NODE *m_pRoot;                                    // Указатель на список записей.
    char *m_pAtlas;                                   // Указатель на Атлас.
  } ATLAS;

// Функция создаёт корневой лист дерева прямоугольников.
// 1-й параметр - указатель на атлас.
// 2-й параметр - ширина добавляемой картинки.
// 3-й параметр - высота добавляемой картинки.
// 4-й параметр - дополнительное пространство для картинки вдоль оси абсцисс.
// 5-й параметр - дополнительное пространство для картинки вдоль оси ординат.
// Возвращаемое значение - указатель на корневой лист дерева.
NODE *atlhlpCreateRoot(ATLAS *pAtlas,unsigned int uiWidth,unsigned int uiHeight,unsigned int dxx,unsigned int dyy);
// Функция создаёт новый пустой лист для дерева прямоугольников и возвращает указатель на него.
// Возвращаемое значение - указатель на созданный пустой лист дерева прямоугольников.
NODE *atlhlpNewNode();
// Функция ищет в атласе место для добавляемой картинки и добавляет эту информацию к непустому дереву прямоугольников.
// 1-й параметр - указатель на дерево прямоугольников.
// 2-й параметр - ширина добавляемой картинки.
// 3-й параметр - высота добавляемой картинки.
// 4-й параметр - дополнительное пространство для картинки вдоль оси абсцисс.
// 5-й параметр - дополнительное пространство для картинки вдоль оси ординат.
// Возвращаемое значение - указатель на лист дерева.
NODE *atlhlpFindPlace(NODE *pRoot,unsigned int uiWidth,unsigned int uiHeight,unsigned int dxx,unsigned int dyy);
// Функция удаляет дерево прямоугольников.
// 1-й параметр - указатель на дерево прямоугольников.
// Возвращаемое значение - нет.
void atlhlpDestroyNode(NODE *pRoot);

/******************************************************************************
*                      Интерфейсные функции - НАЧАЛО                          *
******************************************************************************/
// Функция создаёт пустой атлас и возвращает его обработчик.
// 1-й параметр - ширина атласа в пикселях.
// 2-й параметр - высота атласа в пикселях.
// 3-й параметр - количество байт на пиксель.
// 4-й параметр - количество пустого пространства в пикселях, которым обраймляется вставляемое изображение вдоль оси абсцисс.
// 5-й параметр - количество пустого пространства в пикселях, которым обраймляется вставляемое изображение вдоль оси ординат.
// Возвращаемое значение - обработчик атласа. В случае ошибки возвращается 0.
// P.S. В случае успешного выполнения функции, пустой атлас заполняется прозрачным белым цветом.
HATLAS atlCreate(unsigned int uiWidth,unsigned int uiHeight,unsigned int uiBytePerPixel,unsigned int dx,unsigned int dy)
  {
    ATLAS *pAtlas=malloc(sizeof(ATLAS)+uiWidth*uiHeight*uiBytePerPixel);

    if(!pAtlas)return 0;
    pAtlas->m_uiBytePerPixel=uiBytePerPixel;
    pAtlas->m_uiWidth=uiWidth;
    pAtlas->m_uiHeight=uiHeight;
    pAtlas->m_uiDX=dx;
    pAtlas->m_uiDY=dy;
    pAtlas->m_pRoot=0;
    // Особенности библиотеки ImageMagick (или особенности моих о ней знаний) таковы, что цвета заполняются обычным образом,
    // а непрозрачность заменяется на прозрачность. То есть заполняя весь атлас значением 0xff,
    // он заполняется прозрачным белым цветом.
    memset((pAtlas->m_pAtlas=(char *)(pAtlas+1)),0xff,uiWidth*uiHeight*uiBytePerPixel);
    return (HATLAS)pAtlas;
  }
// Функция добавляет изображение в атлас и возвращает указатель на прямоугольник, содержащий вставленное изображение.
// 1-й параметр - обработчик атласа.
// 2-й параметр - вставляемое изображение в виде массива байт.
// 3-й параметр - ширина вставляемого изображения в пикселях.
// 4-й параметр - высота вставляемого изображения в пикселях.
// 5-й параметр - имя изображения.
// Возвращаемое значение - указатель на прямоугольник, содержащий вставленное изображение.
//                         В случае ошибки (изображение не было вставлено), возвращает 0.
RECTUI *atlAdd(HATLAS hAtlas,void *pByte,unsigned int uiWidth,unsigned int uiHeight,const char *pName)
  {
    ATLAS *pAtlas=(ATLAS *)hAtlas;
    // Проверяем добавлена ли ужЕ какая-то картинка. Если добавлена, то ищем место для новой. Если нет (то есть это будет первая картинка), то создаём пустую запись.
    NODE *pNode=pAtlas->m_pRoot?atlhlpFindPlace(pAtlas->m_pRoot,uiWidth,uiHeight,(pAtlas->m_uiDX<<1),(pAtlas->m_uiDY<<1)):atlhlpCreateRoot(pAtlas,uiWidth,uiHeight,(pAtlas->m_uiDX<<1),(pAtlas->m_uiDY<<1));

    if(pNode)   // Если место нашлось, то добавляем картинку.
      {
        unsigned int i,uiShift=pAtlas->m_uiBytePerPixel*pAtlas->m_uiWidth,uiString=uiWidth*pAtlas->m_uiBytePerPixel;
        // Указатель pPointer будет указывать на то место в атласе, куда нужно будет скопировать добавляемую картинку.
        char *pPointer=pAtlas->m_pAtlas+((pNode->m_rctui.top+pAtlas->m_uiDY)*pAtlas->m_uiWidth+pNode->m_rctui.left+pAtlas->m_uiDX)*pAtlas->m_uiBytePerPixel;

        for(i=0;i<uiHeight;i++)   // В этом цикле происходит копирование байт из добавляемой картинки в атлас, в найденное место.
          memcpy
            (
              pPointer+i*uiShift,//pAtlas->m_uiBytePerPixel*pAtlas->m_uiWidth,
              (char *)pByte+i*uiString,//uiWidth*pAtlas->m_uiBytePerPixel,
              uiString//uiWidth*pAtlas->m_uiBytePerPixel
            );
#ifdef WIN32
#pragma warning(disable:4996)
#endif  // WIN32
        strcpy((pNode->m_pName=malloc(strlen(pName)+1)),pName);   // Сохраняем имя картинки.
#ifdef WIN32
#pragma warning(default:4996)
#endif  // WIN32
        return &pNode->m_rctui;                                   // Возвращаем указатель на прямоугольник, который занимает картинка в атласе.
      }
    return 0;                                                     // Возвращаем 0, если место под картинку в атласе не нашлось.
  }
// Функция возвращает массив байт атласа.
// 1-й параметр - обработчик атласа.
// Возвращаемое значение - массив байт атласа.
void *atlGetData(HATLAS hAtlas)
  {
    return ((ATLAS *)hAtlas)->m_pAtlas;
  }
// Функция удаляет атлас и очищает выделенную под него память.
// 1-й парамер - обработчик атласа.
void atlDestroy(HATLAS hAtlas)
  {
    ATLAS *pAtlas=(ATLAS *)hAtlas;

    atlhlpDestroyNode(pAtlas->m_pRoot);
    //free(pAtlas);
  }
/******************************************************************************
*                       Интерфейсные функции - КОНЕЦ                          *
******************************************************************************/
/******************************************************************************
*                     Вспомогательные функции - НАЧАЛО                        *
******************************************************************************/
// Функция создаёт корневой лист дерева прямоугольников.
// 1-й параметр - указатель на атлас.
// 2-й параметр - ширина добавляемой картинки.
// 3-й параметр - высота добавляемой картинки.
// 4-й параметр - дополнительное пространство для картинки вдоль оси абсцисс.
// 5-й параметр - дополнительное пространство для картинки вдоль оси ординат.
// Возвращаемое значение - указатель на корневой лист дерева.
NODE *atlhlpCreateRoot(ATLAS *pAtlas,unsigned int uiWidth,unsigned int uiHeight,unsigned int dxx,unsigned int dyy)
  {
    if(pAtlas->m_uiWidth<uiWidth+dxx||pAtlas->m_uiHeight<uiHeight+dyy)return 0;   // Если картинка не помещается в атлас выходим и возвращаем 0.
    {
      NODE *pLeft,*pRight,*pRoot=atlhlpNewNode();                                 // Создаём пустой корневой узел.

      pRoot->m_rctui.left=pRoot->m_rctui.top=0;                                   // Заполняем поля только что созданного узла.
      pRoot->m_rctui.right=uiWidth+dxx-1;                                         // Заполняем поля только что созданного узла.
      pRoot->m_rctui.bottom=uiHeight+dyy-1;                                       // Заполняем поля только что созданного узла.
      pLeft=pRoot->m_pLeft=atlhlpNewNode();                                       // Заполняем поля только что созданного узла.
      pRight=pRoot->m_pRight=atlhlpNewNode();                                     // Заполняем поля только что созданного узла.
      // Выясняем как будем разбивать оставшееся пространство атласа на два прямоугольника после добавления картинки.
      // Это нужно, для того, чтобы получить как можно больший прямоугольник.
      // P.S. Меньший прямоугольник будет добавлен в левую ветвь дерева, бОльший - в правую.
      if(uiWidth+dxx<uiHeight+dyy)
        {
          // Добавлеяемая картинка оказалась высокой. Значит разбиваем так, чтоб бОльший прямоугольник оказался
          // справа от добавленной картинки (при этом он получается максимального размера).
          pLeft->m_rctui.left=0;
          pLeft->m_rctui.top=uiHeight+dyy;
          pLeft->m_rctui.right=uiWidth+dxx-1;
          pLeft->m_rctui.bottom=pAtlas->m_uiHeight-1;
          pRight->m_rctui.left=uiWidth+dxx;
          pRight->m_rctui.top=0;
          pRight->m_rctui.right=pAtlas->m_uiWidth-1;
          pRight->m_rctui.bottom=pAtlas->m_uiHeight-1;
          return pAtlas->m_pRoot=pRoot;
        }
      // Добавляемая картинка оказалась широкой. Значит разбиваем так, чтоб бОльший прямоугольник оказался
      // снизу по отношению к добавленной картинке (при этом он получается максимального размера).
      pLeft->m_rctui.left=uiWidth+dxx;
      pLeft->m_rctui.top=0;
      pLeft->m_rctui.right=pAtlas->m_uiWidth-1;
      pLeft->m_rctui.bottom=uiHeight+dyy-1;
      pRight->m_rctui.left=0;
      pRight->m_rctui.top=uiHeight+dyy;
      pRight->m_rctui.right=pAtlas->m_uiWidth-1;
      pRight->m_rctui.bottom=pAtlas->m_uiHeight-1;
      return pAtlas->m_pRoot=pRoot;
    }
  }
// Функция создаёт новый пустой лист для дерева прямоугольников и возвращает указатель на него.
// Возвращаемое значение - указатель на созданный пустой лист дерева прямоугольников.
NODE *atlhlpNewNode()
  {
    NODE *pNode=malloc(sizeof(NODE));

    pNode->m_pName=0;
    pNode->m_pLeft=pNode->m_pRight=0;
    return pNode;
  }
// Функция ищет в атласе место для добавляемой картинки и добавляет эту информацию к непустому дереву прямоугольников.
// 1-й параметр - указатель на дерево прямоугольников.
// 2-й параметр - ширина добавляемой картинки.
// 3-й параметр - высота добавляемой картинки.
// 4-й параметр - дополнительное пространство для картинки вдоль оси абсцисс.
// 5-й параметр - дополнительное пространство для картинки вдоль оси ординат.
// Возвращаемое значение - указатель на лист дерева.
NODE *atlhlpFindPlace(NODE *pRoot,unsigned int uiWidth,unsigned int uiHeight,unsigned int dxx,unsigned int dyy)
  {
    if(pRoot->m_pName)    // Проверяем не пуст ли лист.
      {
        // Если не пуст - углубляемся вглубь дерева.
        {
          // Рекурсивный вызов функции для поиска места. Вначале ищем слева и только затем,
          NODE *pNode=pRoot->m_pLeft?atlhlpFindPlace(pRoot->m_pLeft,uiWidth,uiHeight,dxx,dyy):0;

          if(pNode)return pNode;
        }
        // в случае неудачи справа (сначала пытаемся заполнить маленький прямоугольник).
        return pRoot->m_pRight?atlhlpFindPlace(pRoot->m_pRight,uiWidth,uiHeight,dxx,dyy):0;
      }
    // Если пуст - работаем с этим узлом.
    //
    // Прверяем, вмещается ли в этот прямоугольник добавляемая картинка. Если нет, возвращаем 0, иначе - идём дальше.
    //
    // Можно убрать преобразование к "int", но тогда нужно вместо ">0" написАть ">pAtlas->(m_uiWidth или m_uiHeight)".
    // Но для этого нужно передавать дополнительный параметр в функцию.
    if((int)(uiWidth-pRoot->m_rctui.right+pRoot->m_rctui.left+dxx-1)>0||(int)(uiHeight-pRoot->m_rctui.bottom+pRoot->m_rctui.top+dyy-1)>0)return 0;
    // Если размеры картинки в точности равны найденному прямоугольнику, то просто возвращаем укзатель на лист,
    // содержащий данные на этот прямоугольник и никаких дополнительных преобразований не требуется.
    if(uiWidth==pRoot->m_rctui.right-pRoot->m_rctui.left-dxx+1&&uiHeight==pRoot->m_rctui.bottom-pRoot->m_rctui.top-dyy+1)return pRoot;
    // Картинка вмещается и она даже меньше найденного прямоугольника. Значит добавляем её сюда и разбиваем этот прямоугольник
    // на два прямоугольника тем же способом, что и в функции "atlhlpCreateRoot".
    pRoot->m_pLeft=atlhlpNewNode();                 // Создаём для левого указателя этого узла пустой лист.
    pRoot->m_pRight=atlhlpNewNode();                // Создаём для правого указателя этого узла пустой лист.
    {
      NODE *pLeft=pRoot->m_pLeft,*pRight=pRoot->m_pRight;

      // P.S. Меньший прямоугольник будет добавлен в левую ветвь дерева, бОльший - в правую.
      if(pRoot->m_rctui.right-pRoot->m_rctui.left-dxx-uiWidth<pRoot->m_rctui.bottom-pRoot->m_rctui.top-dyy-uiHeight)
        {
          // Добавляемая картинка оказалась высокой. Значит разбиваем так, чтоб бОльший прямоугольник оказался
          // справа от добавленной картинки (при этом он получается максимального размера).
          pLeft->m_rctui.left=pRoot->m_rctui.left+uiWidth+dxx;
          pLeft->m_rctui.top=pRoot->m_rctui.top;
          pLeft->m_rctui.right=pRoot->m_rctui.right;
          pLeft->m_rctui.bottom=pRoot->m_rctui.top+uiHeight+dyy-1;
          pRight->m_rctui.left=pRoot->m_rctui.left;
          pRight->m_rctui.top=pRoot->m_rctui.top+uiHeight+dyy;
          pRight->m_rctui.right=pRoot->m_rctui.right;
          pRight->m_rctui.bottom=pRoot->m_rctui.bottom;
        }
       else
        {
          // Добавляемая картинка оказалась широкой. Значит разбиваем так, чтоб бОльший прямоугольник оказался
          // снизу по отношению к добавленной картинке (при этом он получается максимального размера).
          pLeft->m_rctui.left=pRoot->m_rctui.left;
          pLeft->m_rctui.top=pRoot->m_rctui.top+uiHeight+dyy;
          pLeft->m_rctui.right=pRoot->m_rctui.left+uiWidth+dxx-1;
          pLeft->m_rctui.bottom=pRoot->m_rctui.bottom;
          pRight->m_rctui.left=pRoot->m_rctui.left+uiWidth+dxx;
          pRight->m_rctui.top=pRoot->m_rctui.top;
          pRight->m_rctui.right=pRoot->m_rctui.right;
          pRight->m_rctui.bottom=pRoot->m_rctui.bottom;
        }
    }
    pRoot->m_rctui.right=pRoot->m_rctui.left+uiWidth+dxx-1;
    pRoot->m_rctui.bottom=pRoot->m_rctui.top+uiHeight+dyy-1;
    return pRoot;
  }
// Функция удаляет дерево прямоугольников.
// 1-й параметр - указатель на дерево прямоугольников.
// Возвращаемое значение - нет.
// P.S. После вызова функции желательно обнулить параметр.
void atlhlpDestroyNode(NODE *pRoot)
  {
    // Удаляем левую ветвь, если таковая имеется.
    if(pRoot->m_pLeft)atlhlpDestroyNode(pRoot->m_pLeft);
    // Удаляем правую ветвь, если таковая имеется.
    if(pRoot->m_pRight)atlhlpDestroyNode(pRoot->m_pRight);
    free(pRoot->m_pName);
    free(pRoot);
  }
/******************************************************************************
*                      Вспомогательные функции - КОНЕЦ                        *
******************************************************************************/
