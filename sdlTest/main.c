#include <proto/exec.h>
#include <proto/intuition.h>
#define NO_INLINE_STDARG
#include <proto/muimaster.h>
#undef NO_INLINE_STDARG
#include <proto/utility.h>
#include <proto/icon.h>
#include <clib/alib_protos.h>
#include "g_misc.h"
#include <string.h>

#define OPTION_USE_CLIB2 0
#include <workbench/startup.h>
#if OPTION_USE_CLIB2
#include <stdlib_headers.h>
#endif

#define MUI_LIB_VERSION 19L

#define ENABLE_SDL

#ifdef ENABLE_SDL
#include <SDL_stdinc.h>
#include <SDL.h>
extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;
#else 
__attribute__((externally_visible)) struct GfxBase *GfxBase = NULL;
__attribute__((externally_visible)) struct IntuitionBase *IntuitionBase = NULL;
#endif


// We need to declare the libraries as "externally_visible" since we are using the option -fwhole-program in the Makefile
__attribute__((externally_visible)) struct ExecBase *SysBase;
__attribute__((externally_visible)) struct DosLibrary *DOSBase = NULL;
__attribute__((externally_visible)) struct UtilityBase *UtilityBase = NULL;
__attribute__((externally_visible)) struct Library *CxBase = NULL;
__attribute__((externally_visible)) struct Library *IconBase = NULL;
struct Library *MUIMasterBase = NULL;

#if OPTION_USE_CLIB2
// Declare all clib2 constructors and destructors
extern int __ctor_stdlib_memory_init(void);
extern int __ctor_stdlib_program_name_init(void);
extern int __ctor_stdio_init(void);
extern int __ctor_stdio_file_init(void);
extern int __ctor_math_init(void);
extern int __ctor_socket_init(void);
extern int __ctor_arg_init(void);
extern int __ctor_rexxvars_init(void);
extern int __ctor_dirent_init(void);
extern int __ctor_locale_init(void);
extern int __ctor_clock_init(void);
extern int __ctor_unistd_init(void);
extern int __ctor_timer_init(void);
extern int __ctor_usergroup_init(void);

extern int __dtor_usergroup_exit();
extern int __dtor_timer_exit();
extern int __dtor_unistd_exit();
extern int __dtor_locale_exit();
extern int __dtor_dirent_exit();
extern int __dtor_rexxvars_exit();
extern int __dtor_socket_exit();
extern int __dtor_math_exit();
extern int __dtor_workbench_exit();
extern int __dtor_stdio_exit();
extern int __dtor_stdlib_program_name_exit();
extern int __dtor_stdlib_memory_exit();
extern int __dtor___wildcard_expand_exit();
extern int __dtor_alloca_exit();
extern int __dtor___setenv_exit();
extern int __dtor___chdir_exit();
#endif

#if OPTION_USE_CLIB2
// Calls all constructors
static VOID fcntCallCtor(void)
{	
	__ctor_stdlib_memory_init();
	__ctor_stdlib_program_name_init();
	__ctor_stdio_init();
	__ctor_stdio_file_init();
	__ctor_math_init();
	__ctor_socket_init();
	__ctor_arg_init();
	__ctor_rexxvars_init();
	__ctor_dirent_init();
	__ctor_locale_init();
	__ctor_clock_init();
	__ctor_unistd_init();
	__ctor_timer_init();
	__ctor_usergroup_init();
}

// Calls all destructors
static VOID fcntCallDtor(void)
{	
	__dtor___wildcard_expand_exit();
	__dtor_alloca_exit();
	__dtor___setenv_exit();
	__dtor___chdir_exit();
	__dtor_usergroup_exit();
	__dtor_timer_exit();
	__dtor_unistd_exit();
	__dtor_locale_exit();
	__dtor_dirent_exit();
	__dtor_rexxvars_exit();
	__dtor_socket_exit();
	__dtor_math_exit();
	__dtor_workbench_exit();
	__dtor_stdio_exit();
	__dtor_stdlib_program_name_exit();
	__dtor_stdlib_memory_exit();
}
#endif

// Global SDL Vars
#ifdef ENABLE_SDL
static SDL_Surface*    	Surf_Canvas = NULL;
static SDL_Surface*		Surf_Picture = NULL;
static SDL_TimerID 		my_timer_id = 0;
static SDL_Joystick*	joystick = NULL;
#endif

// We need to redefine clib api
__attribute__((externally_visible)) void* sbrk(int increment) {
    return (void*) -1;  // Indicate failure
}

__attribute__((externally_visible)) int errno;

// We need to redefine some MUI functions because gcc, due to optimisations, is not always pushing all tags on the stack
Object *MUI_NewObject(CONST_STRPTR cl, Tag tags, ...)
{
	Object *ret = NULL;
	va_list param;
	struct TagItem *tagItem;

	va_start(param,tags);

	if((tagItem = CopyTagsToMem(tags,param))) 
	{
		ret = MUI_NewObjectA(cl,(struct TagItem *)tagItem);
		FreeTagMem(tagItem);
	}

	va_end(param);

	return(ret);
}

void MUIShow() {
    Object *app,*window;
    app = ApplicationObject,
        MUIA_Application_Title, "HelloMUI",
        MUIA_Application_Version, "$VER: HelloMUI 1.0",
        SubWindow, window = WindowObject,
            MUIA_Window_Title, "MUI App",
            MUIA_Window_Activate, TRUE,
            WindowContents, TextObject,
                MUIA_Text_Contents, "Hello, Amiga!",
            End,
        End,
    End;

    if (app) {
        set(window,MUIA_Window_Open,TRUE);
        ULONG sigs = 0;
        while ((LONG)DoMethod(app,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit)
		{
			if (sigs)
			{
				sigs = Wait(sigs | SIGBREAKF_CTRL_C);
				if (sigs & SIGBREAKF_CTRL_C) break;
			}
		}
    }
}

#ifdef ENABLE_SDL
void OpenJoystick(void) {
    SDL_JoystickEventState(SDL_ENABLE);
    
    joystick = SDL_JoystickOpen(1);    
}

SDL_Surface* OnLoad(char* File) {
    SDL_Surface* Surf_Temp = NULL;
    SDL_Surface* Surf_Return = NULL;
 
    if((Surf_Temp = SDL_LoadBMP(File)) == NULL) {
        return NULL;
    }
 
    Surf_Return = SDL_DisplayFormat(Surf_Temp);
    SDL_FreeSurface(Surf_Temp);
 
    return Surf_Return;
}

int OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return -1;
    }
     
    SDL_Rect DestR;
    
    DestR.x = X;
    DestR.y = Y;   
   
    //printf("Calling SDL_BlitSurface()\n");        
    SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);
  
   /*SDL_BlitScaled(SDL_Surface*    src,
                   const SDL_Rect* srcrect,
                   SDL_Surface*    dst,
                   SDL_Rect*       dstrect)*/
  
    return 1;
}  

int SDLShow(char **argv) {
	struct DiskObject *diskObject;
	struct WBStartup* wbStartup;
	wbStartup = (struct WBStartup*)argv;
	Uint32 flags = 0;
	char *toolType;
	Uint32 startTime;
	int i; 
	SDL_PixelFormat format;
	SDL_Rect **sizes;
	int quit = 0;
	SDL_Event event;

	diskObject = GetDiskObject((char*)wbStartup->sm_ArgList[0].wa_Name);
    if (diskObject != NULL) {
		toolType = (char*)FindToolType(diskObject->do_ToolTypes, "FULLSCREEN");
        if (toolType != NULL) {
            flags |= SDL_FULLSCREEN;
        }
		
		toolType = (char*)FindToolType(diskObject->do_ToolTypes, "CLOSE_WB");
        if (toolType != NULL) {
            SDL_putenv("SDL_CLOSE_WB=1");
        }
        
        toolType = (char*)FindToolType(diskObject->do_ToolTypes, "DISPLAY_MODE");
        if (toolType != NULL) {
            if (strcmp(toolType, "NTSC") == 0) {
                SDL_putenv("SDL_DISPLAY_MODE=NTSC");
            } else if (strcmp(toolType, "PAL") == 0) {
                SDL_putenv("SDL_DISPLAY_MODE=PAL");
            }
        }
        
        toolType = (char*)FindToolType(diskObject->do_ToolTypes, "RELATIVE_MOUSE");
        if (toolType != NULL) {
            SDL_putenv("SDL_MOUSE_RELATIVE=1");
        }
	}
	SDL_putenv("SDL_AUDIO_PRIORITY=1");


    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        cleanUPSDL();
        return EXIT_FAILURE;
    }     
    
    startTime = SDL_GetTicks();
    printf("start time = %u\n", startTime);

	OpenJoystick();

	format.BitsPerPixel = 8;
    sizes = SDL_ListModes(&format, flags);
    if ( sizes == (SDL_Rect **)0 ) {
		printf("No sizes supported at this bit-depth\n");
	} else {              
        for ( i=0; sizes[i]; ++i ) {
            printf("Supported size w = %d h = %d\n", sizes[i]->w, sizes[i]->h);
        }
    }    

    if ((Surf_Canvas = SDL_SetVideoMode(320, 240, 8, flags)) == NULL) {
        printf("Could not set video mode: %s\n", SDL_GetError());
        cleanUPSDL();
        return EXIT_FAILURE;
    }
    
    SDL_WM_SetCaption("SDL_Test", "SDL_Test");

	// Create a thread
    /*thread = SDL_CreateThread(TestThread, (void *)NULL);
    if (NULL == thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
    } else {
        // Wait for the thread to complete. The thread functions return code will
        //       be placed in the "threadReturnValue" variable when it completes.
        //
        SDL_WaitThread(thread, &threadReturnValue);
        printf("\nThread returned value: %d\n", threadReturnValue);
    }*/
    
    // Create a Timer callback.
    //my_timer_id = SDL_AddTimer(timerCallbackDelay, TestTimerCallbackFunc, my_callback_param);
	printf("Calling OnLoad()\n");
    if ((Surf_Picture = OnLoad("splash_rtg_240_8.bmp")) == NULL) {
        printf("Could not load bmp: %s\n", SDL_GetError());
        cleanUPSDL();
        return 1;
    }

	/*
     * Palettized screen modes will have a default palette (a standard
     * 8*8*4 colour cube), but if the image is palettized as well we can
     * use that palette for a nicer colour matching
     */
    if (Surf_Picture->format->palette && Surf_Canvas->format->palette) {
        SDL_SetColors(Surf_Canvas, Surf_Picture->format->palette->colors, 0, Surf_Picture->format->palette->ncolors);
    }    
             
    OnDraw(Surf_Canvas, Surf_Picture, 0, 0);

	//SDL_UpdateRect(Surf_Canvas, 0, 0, 320, 200);
    SDL_Flip(Surf_Canvas);

	quit = 0;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
            
            	case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = 1;
                    }
                    break;
                     
            	case SDL_JOYAXISMOTION:
                    if( event.jaxis.axis == 0) 
                    {
                         if (event.jaxis.value == 32767) {
                            printf("Right movement code\n");
                        } else if (event.jaxis.value == -32768) {
                            printf("Left movement code\n");
                        }
                    }
            
                    if( event.jaxis.axis == 1) 
                    {
                        if (event.jaxis.value == 32767) {
                            printf("Down movement code\n");
                            printf("current time = %u\n", SDL_GetTicks());
                        } else if (event.jaxis.value == -32768) {
                            printf("Up movement code\n");
                        }
                    }
                    break;
            }
        }  
    }
    
    printf("end time = %u\n", startTime - SDL_GetTicks());

    // clean-up
    cleanUPSDL();

    printf("EXIT\n");
    return EXIT_SUCCESS;
}
#endif

#ifdef ENABLE_SDL
void cleanUPSDL() {
	if (Surf_Picture) {
        SDL_FreeSurface(Surf_Picture);
        Surf_Picture = NULL;
    } 
        
    if (Surf_Canvas) {
        SDL_FreeSurface(Surf_Canvas);
        Surf_Canvas = NULL;
    }
    
    if (my_timer_id) {
        SDL_RemoveTimer(my_timer_id);
        my_timer_id = 0;
    }  
    
    if (joystick) {
        SDL_JoystickClose(joystick);
        joystick = NULL;
    } 
              
    printf("Calling SDL_Quit()\n");    
    SDL_Quit();  
}
#endif

void cleanUpMUI(void)
{
	if(IntuitionBase != NULL)
		CloseLibrary((struct Library *)IntuitionBase);

	if(DOSBase != NULL)
		CloseLibrary((struct Library *)DOSBase);

	if(UtilityBase != NULL)
		CloseLibrary((struct Library *)UtilityBase);

	if(GfxBase != NULL)
		CloseLibrary((struct Library *)GfxBase);

	if(CxBase != NULL)
		CloseLibrary(CxBase);

	if(IconBase != NULL)
		CloseLibrary(IconBase);

	if(MUIMasterBase != NULL)
		CloseLibrary(MUIMasterBase);
}

int main(int argc, char **argv) 
{
	SysBase = *((struct ExecBase**)4UL);
	struct Process *proc;
	struct WBStartup *wbmsg = NULL;
	struct DiskObject *diskObject;
	int res = 0;
	
	// We check if our program has been started by the workbench or by the shell
	proc = (struct Process *)FindTask(NULL);

	if (proc->pr_CLI == NULL) {
		// We have been launched by workbench: we wait for the startup message
		WaitPort(&proc->pr_MsgPort);
		wbmsg = (struct WBStartup *)GetMsg(&proc->pr_MsgPort);
		diskObject = GetDiskObject((char*)wbmsg->sm_ArgList[0].wa_Name);
	}

#if OPTION_USE_CLIB2
	__WBenchMsg = wbmsg; // this variable is used in __ctor_arg_init()
	__exit_blocked = FALSE; // this will push exit() and similar functions to longjmp to target set by setjmp()
#endif

	// We open the libraries (required since we are linking with alib; please see https://github.com/jyoberle/alib for details)
	IntuitionBase = (struct IntuitionBase *)OpenLibrary((CONST_STRPTR)"intuition.library",0L);
	DOSBase = (struct DosLibrary *)OpenLibrary((CONST_STRPTR)"dos.library",0L);
	UtilityBase = (struct UtilityBase *)OpenLibrary((CONST_STRPTR)"utility.library",0L);
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
	CxBase = OpenLibrary("commodities.library",0);
	IconBase = OpenLibrary("icon.library",0);
	MUIMasterBase = OpenLibrary("muimaster.library",MUI_LIB_VERSION);

	if(IntuitionBase && DOSBase && UtilityBase && GfxBase && CxBase && IconBase && MUIMasterBase)
	{
#if OPTION_USE_CLIB2
		__UtilityBase = UtilityBase; // we need to add this because of macro DECLARE_UTILITYBASE()

		if(setjmp(__exit_jmp_buf) != 0)
			goto out; // target for exit

		fcntCallCtor(); // call the constructors
#endif
#ifdef ENABLE_SDL
		// Manage SDL
		res = SDLShow(argv);
#else
		// If all libs are available, we open the MUI window
		MUIShow();
#endif	

#if OPTION_USE_CLIB2
		printf("This is an example of use of clib2\n");

		out:
		fcntCallDtor(); // call the destructors
#endif
	}

	// Before leaving, we close all libraries
	#ifndef ENABLE_SDL
	cleanUpMUI();
	#endif

	if(wbmsg != NULL)
	{
		// If needed, we reply to the workbench message
		Forbid();
		ReplyMsg((struct Message *)wbmsg);
	}
	return res;
}

__attribute__((used)) __attribute__((section(".text.unlikely"))) void _start(int argc, char **argv)
{
	main(argc,argv);
}