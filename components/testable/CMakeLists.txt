set(app_src mean.c)
set(pre_req testable driver)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "include"
                    REQUIRES ${pre_req})
