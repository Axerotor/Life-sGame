#include "Text.h";


namespace Freetype {


	// Эта функция возвращает число в степени два, большее, чем число a
	inline int next_p2(int a)
	{
		int rval = 1;
		// rval<<=1 это лучше чем rval*=2;
		while (rval < a) rval <<= 1;
		return rval;
	}


	// Создает список отображения на базе данного символа

	void make_dlist(FT_Face face, char ch, GLuint list_base, GLuint * tex_base) {

		// Первая вещь, которую нам надо сделать, это вывести наш символ
		// в растр. Это делается набором команд FreeType
		// Загрузить глифы для каждого символа.
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
			throw std::runtime_error("FT_Load_Glyph failed");


		// Поместить глиф в объект.
		FT_Glyph glyph;

		if (FT_Get_Glyph(face->glyph, &glyph))
			throw std::runtime_error("FT_Get_Glyph failed");

		// Конвертировать глиф в растр.
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		// С помощью этой ссылки, получаем легкий доступ до растра.
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;


		//Вычислим ширину и высоту нашего растра
		int width = next_p2(bitmap.width);
		int height = next_p2(bitmap.rows);

		//Выделим память для данных текстуры
		GLubyte* expanded_data = new GLubyte[2 * width*height];

		//Поместим данные в расширение растр
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				expanded_data[2 * (i + j * width)] = expanded_data[2 * (i + j * width) + 1] =
					(i >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[i + bitmap.width * j];
			}
		}

		//Установим параметры текстуры:
		glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Создаём текстуру 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA,
			GL_UNSIGNED_BYTE, expanded_data);

		//Удаление промежуточных данных
		delete[] expanded_data;

		//Создаём список отображения
		glNewList(list_base + ch, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, tex_base[ch]);

		//Сдвинем символ вправо на расстояние между ним и сомволом до него
		glTranslatef(bitmap_glyph->left, 0, 0);

		//Сдвинем вниз если растр уходит вниз строки. 
		//Для таких символов как g & y
		glPushMatrix();
		glTranslatef(0, bitmap_glyph->top - bitmap.rows, 0);


		// Вычислим какая часть нашей текстуры будет заполнена пустым пространством.
		// Мы рисуем только ту часть текстуры, в которой находится символ, и сохраняем
		// информацию в переменных x и y, затем, когда мы рисуем четырехугольник
		// мы будем только ссылаться на ту часть текстуры, в которой непосредственно
		// содержится символ.
		float x = (float)bitmap.width / (float)width,
			y = (float)bitmap.rows / (float)height;

		//Рисуем текструрированный четырехугольник
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex2f(0, bitmap.rows);
		glTexCoord2d(0, y); glVertex2f(0, 0);
		glTexCoord2d(x, y); glVertex2f(bitmap.width, 0);
		glTexCoord2d(x, 0); glVertex2f(bitmap.width, bitmap.rows);
		glEnd();
		glPopMatrix();
		glTranslatef(face->glyph->advance.x >> 6, 0, 0);

		//Завершим создание списка отображения
		glEndList();
	}

	void text::init(const char* fname, unsigned int h) {
		//Выделим память для идентификаторов текстуры
		textures = new GLuint[128];

		this->h = h;

		//Инициализация библиотеку FreeType
		FT_Library library;
		if (FT_Init_FreeType(&library))
			throw std::runtime_error("FT_Init_FreeType failed");

		//Объект для хранения шрифта
		FT_Face face;

		//Загрузим шрифт из файла
		if(FT_New_Face(library,fname,0,&face))
			throw std::runtime_error("FT_New_Face failed(there is probably a problem with your font file");
		
		// По некоторым причинам FreeType измеряет размер шрифта в терминах 1/64 пикселя.
		// Таким образом, для того чтобы сделать шрифт выстой h пикселей, мы запрашиваем размер h*64.
		// (h << 6 тоже самое что и h*64)
		FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);


		//Выделяем память для всех текстур и списков отображения
		list_base = glGenLists(128);
		glGenTextures(128, textures);

		//Создаём списки отображения шрифтов
		for (unsigned char i = 0; i < 128; i++)
			make_dlist(face, i, list_base, textures);

		//Уничтожим шрифт и библиотеку
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}

	void text::clean() {
		glDeleteLists(list_base, 128);
		glDeleteTextures(128, textures);
		delete[] textures;
	}


	//Функция, которая сохраняет матрицу проекции, 
	//затем делает мировые координаты идентичными с координатами окна

	inline void pushScreenCoordinateMatrix() {
		glPushAttrib(GL_TRANSFORM_BIT);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glm::ortho(viewport[0], viewport[2], viewport[1], viewport[3]);
		glPopAttrib();
	}

	//Функция, которая восстанавливает координаты матрицы проекции
	inline void pop_projection_matrix() {
		glPushAttrib(GL_TRANSFORM_BIT);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}

	void print(const text &ft_font, float x, float y, const char *fmt, ...) {

		//Преобразовываем систему координат, в которых расстояние измеряется в писелях
		pushScreenCoordinateMatrix();

		GLuint font = ft_font.list_base;
		//Сделаем высоту немного больше, чтобы оставить место между линиями
		float h = ft_font.h / .63f;
		char text[256];   
		va_list ap;   //Указатель на лист аргументов
		if (fmt == NULL)
			*text = 0;
		else {
			va_start(ap, fmt);   //Разбор строки на переменные
			vsprintf_s(text, fmt, ap);  //Инвертировать символы в числа
			va_end(ap);   //результат сохранить в текст
		}

		//Разделим текст на строки
		const char *start_line = text;
		std::vector<string> lines;
		for (const char *c = text; *c; c++) {
			if (*c == '\n') {
				string line;
				for (const char *n = start_line; n < c; n++) line.append(1, *n);
				lines.push_back(line);
				start_line = c + 1;

			}

			if (start_line) {
				string line;
				for (const char *n = start_line; n < c; n++) line.append(1, *n);
				lines.push_back(line);
			}
		}
		glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glListBase(font);

		float modelview_matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

		//На каждой строчке мы сбрасываем матрицу вида модели
		//Поэтому строки будут начинаться с правильной позиции
		for (int i = 0; i < lines.size(); i++) {
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(x, y - h * i, 0);
			glMultMatrixf(modelview_matrix);

			glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
			glPopMatrix();
		}

		glPopAttrib();
		pop_projection_matrix();

	}
};