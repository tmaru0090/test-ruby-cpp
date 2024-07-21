/*
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ruby.h>

#include <map>
#include <string>



// グローバル変数としてウィンドウとテクスチャを保持
sf::RenderWindow* window;
std::map<std::string, sf::Texture> textures;  // 複数のテクスチャを管理するマップ
std::map<std::string,sf::Music> music;        // 複数の音声ファイルを管理するマップ

// ロード済みの音声(キー)を利用して再生する関数
VALUE play_music(VALUE self,VALUE key) {
    const char* c_key = StringValueCStr(key);
    if (music.find(c_key) == music.end()) {
        printf("キー%sに対応する画像が見つかりません。\n", c_key);
        return Qnil;
    }
    music[c_key].play();
    return Qnil;

}

VALUE load_music(VALUE self, VALUE key, VALUE filePath) {
    const char* c_key = StringValueCStr(key);
    const char* path = StringValueCStr(filePath);

    sf::Music m;
    if (!m.openFromFile(path)) {
        printf("音声ファイル%sの読み込みに失敗しました。\n", path);
        return Qnil;
    }

    music[c_key] = m;  // マップに音声ファイルを保存
    printf("音声ファイル%sをキー%sでロードしました。\n", path, c_key);
    return Qnil;

}
// 画像をロードする関数
VALUE load_image(VALUE self, VALUE key, VALUE filePath) {
    const char* c_key = StringValueCStr(key);
    const char* path = StringValueCStr(filePath);

    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        printf("画像ファイル%sの読み込みに失敗しました。\n", path);
        return Qnil;
    }

    textures[c_key] = texture;  // マップにテクスチャを保存
    printf("画像ファイル%sをキー%sでロードしました。\n", path, c_key);
    return Qnil;
}

// ロード済みの画像(キー)を利用して描画する関数
VALUE draw_image(VALUE self, VALUE key, VALUE x, VALUE y, VALUE angle) {
    const char* c_key = StringValueCStr(key);
    int c_x = NUM2INT(x);
    int c_y = NUM2INT(y);
    float c_angle = NUM2DBL(angle);

    if (textures.find(c_key) == textures.end()) {
        printf("キー%sに対応する画像が見つかりません。\n", c_key);
        return Qnil;
    }

    sf::Sprite sprite;
    sprite.setTexture(textures[c_key]);
    sprite.setPosition(sf::Vector2f(c_x, c_y));
    sprite.setRotation(c_angle);  // 画像を回転させる
    window->draw(sprite);
    return Qnil;
}
// ウィンドウサイズの設定
VALUE set_window_size(VALUE self,VALUE width,VALUE height){
	int c_width,c_height;
	c_width = NUM2INT(width);
	c_height = NUM2INT(height);
	window->setSize(sf::Vector2u(c_width,c_height));		
    	return Qnil;
}
int main(int argc, char **argv) {
    
    // ウィンドウを作成
    sf::RenderWindow win(sf::VideoMode(640, 480), "SFML Window");
    window = &win;

    // Rubyインタプリタの初期化
    ruby_init();
    ruby_init_loadpath();
    rb_define_global_function("load_image", load_image, 2);
    rb_define_global_function("draw_image", draw_image, 4);
    rb_define_global_function("load_music", load_music, 2);
    rb_define_global_function("play_music", play_music, 1);
    rb_define_global_function("set_window_size", set_window_size, 2);
    int state = 0;

    // Rubyスクリプトの読み込みと実行
    rb_load_protect(rb_str_new_cstr("./script.rb"), 0, &state);
    if (state) {
        // エラーが発生した場合の処理
        VALUE err = rb_errinfo();
        rb_set_errinfo(Qnil); // エラー情報をクリア
        fprintf(stderr, "Error: %s\n", RSTRING_PTR(rb_obj_as_string(err)));
    }

    // Rubyスクリプト内の初期化関数の呼び出し
    rb_funcall(Qnil, rb_intern("init"), 0);

    // メインループ
    while (window->isOpen()) {
        // イベント処理
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }

        // 画面をクリア
        window->clear();
        
        // Rubyスクリプト内の描画関数と更新関数の呼び出し
        rb_funcall(Qnil, rb_intern("update"), 0);
        rb_funcall(Qnil, rb_intern("draw"), 0);

        // 画面に表示
        window->display();
    }

    // Rubyインタプリタの終了
    ruby_finalize();

    return 0;
}*/




#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ruby.h>

#include <map>
#include <string>
#include <memory>
#include <uuid/uuid.h>
#include <stdio.h>

// グローバル変数としてウィンドウとリソースを保持
sf::RenderWindow* window;
std::map<std::string, sf::Texture> textures;  // 複数のテクスチャを管理するマップ
std::map<std::string, std::shared_ptr<sf::Music>> music;  // 複数の音声ファイルを管理するマップ
std::map<std::string, sf::Font> fonts;  // 複数のフォントを管理するマップ
std::map<int, bool> keyStates; // キーコードとその状態 (押されているか) のマップ
bool endFlag = false;

// ランダムにバイト列を生成
void generate_random_bytes(char *buffer, size_t length) {
    uuid_t uuid;
    uuid_generate_random(uuid);

    // UUIDの最初の`length`バイトを`buffer`にコピー
    memcpy(buffer, uuid, length);
}

// ロード済みの音声(キー)を利用して再生する関数
VALUE play_music(VALUE self, VALUE key) {
    const char* c_key = StringValueCStr(key);
    if (music.find(c_key) == music.end()) {
        printf("キー%sに対応する音声が見つかりません。\n", c_key);
        return Qnil;
    }
    music[c_key]->play();
    return Qnil;
}

// 音声ファイルをロードする関数
VALUE load_music(VALUE self, VALUE filePath) {
    char c_key[36];
    generate_random_bytes(c_key, 30);

    const char* path = StringValueCStr(filePath);
    auto m = std::make_shared<sf::Music>();
    if (!m->openFromFile(path)) {
        printf("音声ファイル%sの読み込みに失敗しました。\n", path);
        return Qnil;
    }

    music[c_key] = m;  // マップに音声ファイルを保存
    printf("音声ファイル%sをキー%sでロードしました。\n", path, c_key);
    VALUE r_key = rb_str_new_cstr(c_key);
    return r_key;
}

// 画像をロードする関数
VALUE load_image(VALUE self, VALUE filePath) {
    char c_key[36];
    generate_random_bytes(c_key, 30);

    const char* path = StringValueCStr(filePath);

    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        printf("画像ファイル%sの読み込みに失敗しました。\n", path);
        return Qnil;
    }

    textures[c_key] = texture;  // マップにテクスチャを保存
    printf("画像ファイル%sをキー%sでロードしました。\n", path, c_key);
    VALUE r_key = rb_str_new_cstr(c_key);
    return r_key;
}

// ロード済みの画像(キー)を利用して描画する関数
VALUE draw_image(VALUE self, VALUE key, VALUE x, VALUE y, VALUE angle) {
    const char* c_key = StringValueCStr(key);
    int c_x = NUM2INT(x);
    int c_y = NUM2INT(y);
    float c_angle = NUM2DBL(angle);

    if (textures.find(c_key) == textures.end()) {
        printf("キー%sに対応する画像が見つかりません。\n", c_key);
        return Qnil;
    }

    sf::Sprite sprite;
    sprite.setTexture(textures[c_key]);
    sprite.setPosition(sf::Vector2f(c_x, c_y));
    sprite.setRotation(c_angle);  // 画像を回転させる
    window->draw(sprite);
    return Qnil;
}

// フォントをロードする関数
VALUE load_font(VALUE self, VALUE filePath) {
    char c_key[36];
    generate_random_bytes(c_key, 30);

    const char* path = StringValueCStr(filePath);

    sf::Font font;
    if (!font.loadFromFile(path)) {
        printf("フォントファイル%sの読み込みに失敗しました。\n", path);
        return Qnil;
    }

    fonts[c_key] = font;  // マップにフォントを保存
    printf("フォントファイル%sをキー%sでロードしました。\n", path, c_key);
    VALUE r_key = rb_str_new_cstr(c_key);
    return r_key;
}

// テキストを描画する関数
VALUE draw_text(VALUE self, VALUE x, VALUE y, VALUE text, VALUE font_key, VALUE size) {
    int c_x = NUM2INT(x);
    int c_y = NUM2INT(y);
    const char* c_text = StringValueCStr(text);
    const char* c_font_key = StringValueCStr(font_key);
    int c_size = NUM2INT(size);

    if (fonts.find(c_font_key) == fonts.end()) {
        printf("キー%sに対応するフォントが見つかりません。\n", c_font_key);
        return Qnil;
    }

    sf::Text sf_text;
    sf_text.setFont(fonts[c_font_key]);
    sf_text.setString(c_text);
    sf_text.setCharacterSize(c_size);
    sf_text.setPosition(sf::Vector2f(c_x, c_y));
    
    window->draw(sf_text);
    return Qnil;
}

// ウィンドウサイズの設定
VALUE set_window_size(VALUE self, VALUE width, VALUE height) {
    int c_width = NUM2INT(width);
    int c_height = NUM2INT(height);
    window->setSize(sf::Vector2u(c_width, c_height));
    return Qnil;
}

// キー状態を更新する関数
void update_key_states(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
        keyStates[event.key.code] = true;
    } else if (event.type == sf::Event::KeyReleased) {
        keyStates[event.key.code] = false;
    }
}

// キーが押されているかチェックする関数
VALUE is_key_pressed(VALUE self, VALUE key_code) {
    int code = NUM2INT(key_code);
    auto it = keyStates.find(code);
    if (it != keyStates.end()) {
        return it->second ? Qtrue : Qfalse;
    }
    return Qfalse; // デフォルトでは押されていない
}

bool rb_value_to_bool(VALUE value) {
    return RTEST(value) ? true : false;
}
// メインループを抜けるフラグを設定
VALUE set_end_flag(VALUE self,VALUE flag){
     endFlag = rb_value_to_bool(flag);
     return Qnil;
}
// SFMLのキーボードキーコードをRubyに公開する関数
void define_sfml_keycodes() {
    rb_define_const(rb_cObject, "KEY_A", INT2NUM(sf::Keyboard::A));
    rb_define_const(rb_cObject, "KEY_B", INT2NUM(sf::Keyboard::B));
    rb_define_const(rb_cObject, "KEY_C", INT2NUM(sf::Keyboard::C));
    // 必要なキーコードを追加する
    // ...
    rb_define_const(rb_cObject, "KEY_Z", INT2NUM(sf::Keyboard::Z));
    rb_define_const(rb_cObject, "KEY_LEFT", INT2NUM(sf::Keyboard::Left));
    rb_define_const(rb_cObject, "KEY_RIGHT", INT2NUM(sf::Keyboard::Right));
    rb_define_const(rb_cObject, "KEY_UP", INT2NUM(sf::Keyboard::Up));
    rb_define_const(rb_cObject, "KEY_DOWN", INT2NUM(sf::Keyboard::Down));
    rb_define_const(rb_cObject, "KEY_ESCAPE", INT2NUM(sf::Keyboard::Escape));
    return;
}
void define_global_func(){
    rb_define_global_function("load_image", load_image, 1);
    rb_define_global_function("draw_image", draw_image, 4);
    rb_define_global_function("load_music", load_music, 1);
    rb_define_global_function("play_music", play_music, 1);
    rb_define_global_function("set_window_size", set_window_size, 2);
    rb_define_global_function("load_font", load_font, 1);
    rb_define_global_function("draw_text", draw_text, 5);
    rb_define_global_function("is_key_pressed", is_key_pressed, 1);
    rb_define_global_function("set_end_flag", set_end_flag, 1);
}
int main(int argc, char **argv) {
    // ウィンドウを作成
    sf::RenderWindow win(sf::VideoMode(640, 480), "SFML Window");
    window = &win;

    // キー状態の初期化
    keyStates.clear();

    // Rubyインタプリタの初期化
    ruby_init();
    ruby_init_loadpath();
    define_global_func();
    define_sfml_keycodes();

    int state = 0;

    // Rubyスクリプトの読み込みと実行
    rb_load_protect(rb_str_new_cstr("./script.rb"), 0, &state);
    if (state) {
        // エラーが発生した場合の処理
        VALUE err = rb_errinfo();
        rb_set_errinfo(Qnil); // エラー情報をクリア
        fprintf(stderr, "Error: %s\n", RSTRING_PTR(rb_obj_as_string(err)));
    }

    // Rubyスクリプト内の初期化関数の呼び出し
    rb_funcall(Qnil, rb_intern("init"), 0);

    // メインループ
    while (window->isOpen()) {
        // イベント処理
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            } else {
                // キー入力イベントを更新
                update_key_states(event);
            }
        }
	// エンドフラグがオンなら
	if(endFlag){
		// 終了
		break;
	}
        // 画面をクリア
        window->clear();

        // Rubyスクリプト内の描画関数と更新関数の呼び出し
        rb_funcall(Qnil, rb_intern("update"), 0);
        rb_funcall(Qnil, rb_intern("draw"), 0);

        // 画面に表示
        window->display();
    }

    // Rubyインタプリタの終了
    ruby_finalize();

    return 0;
}
