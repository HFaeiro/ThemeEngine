# ThemeEngine
My Custom win32 Button/edit and listview lib/procedures.


Usage:

if using just the list view you will have to use

InitializeListView(HINSTANCE, HBRUSH); //HBRUSH is for background color.

if you plan on using the full theme engine with custom buttons and edit controls you must also use 

Theme::Initialize(HINSTANCE, HBRUSH);

there is a function called Theme::CWindow. this takes almost the same params as CreateWindow and must be used to create a Theme::Button or Theme::edit window. 
ex: 

for a button:

Theme::CWindow(r, L"Segoe UI", L"CBUTTON", L"Ok", WS_VISIBLE,
			120, 80, 20, 15, hWnd, (HMENU)IDB_OK);
      
for an edit control:

Theme::CWindow(r, L"Segoe UI", L"CEDIT", L"Port", WS_VISIBLE,
			55, 30, 87, 15, hWnd, (HMENU)IDE_PORT);
   
   
   ---------------USING LISTVIEW--------------
   
   there are a few commands that will be quite useful. 
   
   IDB_ADD - adds a std::vector<std::wstring> to the listview as a ROW through the LPARAM 
   
   Ex.. SendMessage(ListView, IDB_ADD, NULL, std::vector<std::wstring>)
   
   IDB_CLEAR - clears the listview. takes no params
   
   IDR_GETLIST - returns std::vector<std::vector<std::string>> as full listview data.
   
   IDB_SORTDEC - sorts by columns decending. takes column number as LPARAM
   
   IDB_SORTACE - sorts by columns acending ^^
   
   
