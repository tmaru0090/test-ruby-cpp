/*
#include <ruby.h>
#include <stdio.h>
#include <string.h>
#include <SFML/Graphics.hpp>


// C関数の定義

VALUE music_play(VALUE self, VALUE filePath) {
    char program[100] = "mpv ";
    const char* path = StringValueCStr(filePath);
    strncat(program,path,sizeof(program) - strlen(program) -1);
    system(program);
    // ここに関数の処理を記述
    return Qnil;
}


VALUE MusicClass; // MusicClassClass の定義
VALUE music_play(VALUE self, VALUE filePath) {
    char program[100] = "mpv ";
    const char* path = StringValueCStr(filePath);
    strncat(program,path,sizeof(program) - strlen(program) -1);
    system(program);
    // ここに関数の処理を記述
    return Qnil;
}

VALUE music_play_sequence(VALUE self, VALUE directory, VALUE base_name, VALUE start_num, VALUE end_num) {
    char file_path[256]; // Buffer to store the file path
    int start = NUM2INT(start_num); // Convert Ruby VALUE to C int
    int end = NUM2INT(end_num); // Convert Ruby VALUE to C int
    const char *dir = StringValueCStr(directory); // Convert Ruby VALUE to C string
    const char *base = StringValueCStr(base_name); // Convert Ruby VALUE to C string

    for (int i = start; i <= end; i++) {
        snprintf(file_path, sizeof(file_path), "%s/%s_%02d.mp3", dir, base, i);
 	VALUE rb_file_path = rb_str_new_cstr(file_path);
        // Assuming music_play is a function that plays the music file
        music_play(self, rb_file_path);
    }
    return Qnil;
}
void Init_my_extension() {
    MusicClass = rb_define_class("Music", rb_cObject);
    rb_define_method(MusicClass, "music_play", music_play, 1);
    rb_define_method(MusicClass, "music_play_sequence",music_play_sequence, 4);
    rb_gc_register_address(&MusicClass); 
}

int main(int argc, char **argv) {
     // ウィンドウを作成
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");

    // メインループ
    while (window.isOpen()) {
        // イベント処理
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 画面をクリア
        window.clear();

        // 描画処理（ここに描画コードを追加）

        // 画面に表示
        window.display();
    }
    
    // Rubyインタプリタの初期化
    ruby_init();
    ruby_init_loadpath();
    Init_my_extension();
    //rb_define_global_function("music_play",music_play, 1);
    int state =0;
    // Rubyスクリプトの読み込みと実行
    rb_load_protect(rb_str_new_cstr("./script.rb"), 0, &state);
    if (state) {
        // エラーが発生した場合の処理
        VALUE err = rb_errinfo();
        rb_set_errinfo(Qnil); // エラー情報をクリア
        fprintf(stderr, "Error: %s\n", RSTRING_PTR(rb_obj_as_string(err)));
    }
    // Rubyインタプリタの終了
    ruby_finalize();
    return 0;
}
*/


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ruby.h>

// グローバル変数としてウィンドウを保持
sf::RenderWindow* window;
sf::Music* music;
// SFMLの描画関数をラップする関数
VALUE draw_circle(VALUE self) {
    sf::CircleShape circle(50);
    circle.setFillColor(sf::Color::Green);
    window->draw(circle);
    return Qnil;
}
VALUE music_play(VALUE self, VALUE filePath) {
    const char* path = StringValueCStr(filePath);
    if (!music->openFromFile(path)) {
     	printf("音声ファイル%sの読み込みに失敗しました。",path);
	return Qnil;
    }
    music->play();
    // ここに関数の処理を記述
    return Qnil;
}
VALUE draw_image(VALUE self, VALUE filePath) {
    const char* path = StringValueCStr(filePath);
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        printf("画像ファイル%sの読み込みに失敗しました。\n", path);
        return Qnil;
    }
    sf::Sprite sprite;
    sprite.setTexture(texture);
    window->draw(sprite);
    return Qnil;
}
int main(int argc, char **argv) {
    // ウィンドウを作成
    sf::RenderWindow win(sf::VideoMode(640, 480), "SFML Window");
    sf::Music mu;
    window = &win;
    music = &mu;

    // Rubyインタプリタの初期化
    ruby_init();
    ruby_init_loadpath();
    rb_define_global_function("draw_circle",draw_circle, 0);
    rb_define_global_function("music_play",music_play, 1);
    rb_define_global_function("draw_image",draw_image, 1);
    int state = 0;
    // Rubyスクリプトの読み込みと実行
    rb_load_protect(rb_str_new_cstr("./script.rb"), 0, &state);
    if (state) {
        // エラーが発生した場合の処理
        VALUE err = rb_errinfo();
        rb_set_errinfo(Qnil); // エラー情報をクリア
        fprintf(stderr, "Error: %s\n", RSTRING_PTR(rb_obj_as_string(err)));
    }
    // rubyスクリプト内の初期化関数の呼び出し
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
}
