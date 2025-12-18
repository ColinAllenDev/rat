# Function Pointer Table (vtables)       
                                                         
Best if you might support multiple platforms or want   
runtime selection:                                     
```c

// window.h                                            
typedef struct rt_platform_api {                       
  int (*init)(void);                                 
  void (*shutdown)(void);                            
  rt_window* (*create_window)(int, int, const char*);
  // ... etc                                         
} rt_platform_api;                                     
                                                     
// Initialize with specific platform                   
int rt_init_platform(const rt_platform_api* api);

``` 
This is more complex but gives you runtime platform    
selection and makes unit testing much easier (you can  
inject mock implementations).
