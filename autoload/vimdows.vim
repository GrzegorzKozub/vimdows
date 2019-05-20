if &compatible || !has('win32') || !(has('gui_running') || exists('g:GuiLoaded')) || exists('g:vimdows_loaded')
    finish
endif

let s:dll = expand('<sfile>:p:h:h') . '/bin/vimdows' . (has('win64') ? '64' : '32') . '.dll'

function! vimdows#get_screen() abort
    return libcall(s:dll, 'GetScreen', '')
endfunction

if has('nvim')
  let g:vimdows_loaded = 1
  finish
endif

function! vimdows#get_maximized() abort
    return libcallnr(s:dll, 'GetMaximized', '')
endfunction

function! vimdows#get_full_screen() abort
    return libcallnr(s:dll, 'GetFullScreen', '')
endfunction

function! vimdows#maximize() abort
    call libcall(s:dll, 'Maximize', '')
endfunction

function! vimdows#restore() abort
    call libcall(s:dll, 'Restore', '')
endfunction

function! vimdows#enter_full_screen() abort
    call libcall(s:dll, 'EnterFullScreen', '')
endfunction

function! vimdows#exit_full_screen() abort
    call libcall(s:dll, 'ExitFullScreen', '')
endfunction

function! vimdows#fix_background() abort
    call libcall(s:dll, 'FixBackground', strpart(synIDattr(hlID('Normal'), 'bg#'), 1))
endfunction

function! vimdows#maximized_toggle() abort
    if vimdows#get_full_screen() | return | endif
    if vimdows#get_maximized() | call vimdows#restore() | else | call vimdows#maximize() | endif
endfunction

function! vimdows#full_screen_toggle() abort
    if vimdows#get_full_screen()
        call vimdows#exit_full_screen()
        call vimdows#restore_screen(0)
    else
        call vimdows#save_screen()
        call vimdows#enter_full_screen()
    endif
endfunction

function! vimdows#init_screen_memento() abort
    if !exists('g:VIMDOWS_SCREEN["' . v:servername . '"]')
        call vimdows#save_screen()
    endif
endfunction

function! vimdows#save_screen() abort
    if !exists('g:VIMDOWS_SCREEN')
        let g:VIMDOWS_SCREEN = {}
    endif
    let l:maximized = vimdows#get_maximized()
    let l:fullScreen = vimdows#get_full_screen()
    if !l:maximized && !l:fullScreen
        let g:VIMDOWS_SCREEN[v:servername] = [ &columns, &lines, getwinposx(), getwinposy(), 0, 0 ]
    else
        if !exists('g:VIMDOWS_SCREEN["' . v:servername . '"]')
            throw 'Initial screen memento can''t be maximized or full screen'
        endif
        let g:VIMDOWS_SCREEN[v:servername][4] = l:maximized
        let g:VIMDOWS_SCREEN[v:servername][5] = l:fullScreen
    endif
endfunction

function! vimdows#restore_screen(allowFullScreen) abort
    if !exists('g:VIMDOWS_SCREEN') || !exists('g:VIMDOWS_SCREEN["' . v:servername . '"]')
        return
    endif
    let l:screen = g:VIMDOWS_SCREEN[v:servername]
    silent! exe 'set columns=' . l:screen[0] . ' lines=' . l:screen[1]
    silent! exe 'winpos ' . l:screen[2] . ' ' . l:screen[3]
    if l:screen[4]
        call vimdows#maximize()
        call vimdows#fix_background()
    endif
    if l:screen[5] && a:allowFullScreen
        call vimdows#enter_full_screen()
        call vimdows#fix_background()
    endif
endfunction

let g:vimdows_loaded = 1

