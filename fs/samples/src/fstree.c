#include "../include/fstree.h"

FStree * root = NULL;

char * extract_path(char ** copy_path){
	printf("EXTRACT PATH CALLED\n");
    char * retval = (char *)calloc(sizeof(char), 1);
    int retlen = 0;
    char temp;
    char * tempstr;
    temp = **(copy_path);
    while(temp != '\0'){    
        if(temp == '/'){
            if(strlen(*copy_path) > 1){
                (*copy_path)++;
            }
            break;
        }
        tempstr = (char *)calloc(sizeof(char) , (retlen + 2));
        strcpy(tempstr, retval);
        retlen += 1;
        tempstr[retlen - 1] = temp;
        retval = (char *)realloc(retval, sizeof(char) * (retlen + 2));
        strcpy(retval, tempstr);
        (*copy_path)++;
        temp = **(copy_path);
        free(tempstr);
    }
    retval = (char *)realloc(retval, sizeof(char) * (retlen + 1));
    retval[retlen] = '\0';
    return retval;
}

char * reverse(char * str, int mode){
	printf("REVERSE CALLED\n");
    int i;
    int len = strlen(str);
    char * retval = (char *)calloc(sizeof(char), (len + 1));
    for(i = 0; i <= len/2; i++){
        retval[i] = str[len - 1 -i];
        retval[len - i - 1] = str[i];
    }
    if(retval[0] == '/' && mode == 1) {   // if mode is set to 1, then drop the leading '/'. Set to 0 for normal reversing.
        retval++;
    }
    return retval;
}

char * extract_dir(char ** copy_path){
	printf("EXTRACT_DIR CALLED\n");
    char * retval = (char *)calloc(sizeof(char), 1);
    int retlen = 0;
    char temp;
    char * tempstr;
    *copy_path = reverse(*copy_path, 1);    // change "a/b/c" to "c/b/a" and extract content upto the first '/'
    temp = **(copy_path);
    while(temp != '/'){    
        tempstr = (char *)calloc(sizeof(char), (retlen + 2));
        strcpy(tempstr, retval);
        retlen += 1;
        tempstr[retlen - 1] = temp;
        retval = (char *)realloc(retval, sizeof(char) * (retlen + 1));
        strcpy(retval, tempstr);
        (*copy_path)++;
        temp = **(copy_path);
        free(tempstr);
    }
    if(strlen(*copy_path) > 1){
        (*copy_path)++;                     // remove the leading '/' from "/b/a" after extracting 'c'
    }
    retval = (char *)realloc(retval, sizeof(char) * (retlen + 1));
    retval[retlen] = '\0';
    retval = reverse(retval, 0);        
    *(copy_path) = reverse(*(copy_path), 0);
    return retval;
}

FStree * search_node(char * path){
	printf("SEARCH_NODE CALLED\n");
    FStree * temp = root;
    FStree * retval = NULL;
    char * curr_node = NULL;
    int flag = 0, i = 0;
    if(path[0] == '/'){
        path++;
    }
    while(temp != NULL){
        curr_node = extract_path(&path);
        if(strlen(curr_node) == 0){
            break;
        }
        for(i = 0; i < temp->num_children; i++){
            if(strcmp(temp->children[i]->name, curr_node) == 0){
                retval = temp->children[i];
                temp = temp->children[i];
                flag = 1;
            }
        }
        if(!flag){
            return NULL;
        }
        else{
            flag = 0;
        }
    }
    if(retval!=NULL){
    	return retval;
	}
	return NULL;
}

FStree * init_node(const char * path, char * name, FStree * parent,int type){
	printf("INIT_NODE CALLED\n");
    FStree * new = (FStree *)malloc(sizeof(FStree));
    new->path = (char *)calloc(sizeof(char), strlen(path) + 1);
    new->name = (char *)calloc(sizeof(char), strlen(name) + 1);
    strcpy(new->path, (char *)path);
    strcpy(new->name, (char *)name);
    if(type == 1){
    	new->type = "directory";  
        new->permissions = S_IFDIR | 0777;
    }     
    if(type == 0){
	    new->type = "file"; 
    	new->permissions = S_IFREG | 0777; 
    }    
    new->group_id = getgid();
    new->user_id = getuid();
    new->c_time = time(&t);
    new->a_time = time(&t);
    new->m_time = time(&t);
    new->b_time = time(&t);
	new->inode_number = 0;
    new->num_children = 0;
    new->parent = parent;
    new->children = NULL;
    new->fchildren = NULL;
    new->num_files = 0;
    new->size = 0;
    return new;
}

void insert_node(const char * path){
	printf("INSERT_NODE CALLED\n");
    if(root == NULL){
		printf("CREATING ROOT\n");
        root = init_node("/", "root", NULL,1);
        return;
    }
    else{
        char * copy_path = (char *)path;
        char * dir = extract_dir(&copy_path);
        FStree * dir_node = NULL;
        if(strlen(copy_path) == 1){   
            root->num_children++;
            if(root->children == NULL){
                root->children = (FStree **)malloc(sizeof(FStree *));
                root->children[0] = init_node(path, dir, root,1);
            }
            else{
                root->children = (FStree **)realloc(root->children, sizeof(FStree *) * root->num_children);
                root->children[root->num_children - 1] = init_node(path, dir, root,1);
            }
        }
        else{
            dir_node = search_node(copy_path);  
            if(dir_node != NULL){
				if(dir_node->parent!=NULL){
					dir_node->c_time=time(NULL);
					dir_node->m_time=time(NULL);
				}
                dir_node->num_children++;
                dir_node->children = (FStree **)realloc(dir_node->children, sizeof(FStree *) * dir_node->num_children);
                dir_node->children[dir_node->num_children - 1] = init_node(path, dir, dir_node,1);
            }
        }
        return;
    }
    return;
}

void load_node(char * path, char * type, gid_t groupid, uid_t userid, time_t lc_time, time_t lm_time, time_t la_time, time_t lb_time, unsigned long int inode, off_t size, mode_t lpermissions){
    printf("LOAD_NODE CALLED\n");
    if(root == NULL){
		printf("LOADING ROOT NODE!\n");
        root = init_node("/", "root", NULL, 1);
		root->group_id = groupid;
		root->user_id = userid;
		root->c_time = lc_time;
		root->a_time = la_time;
		root->m_time = lm_time;
		root->b_time = lb_time;
		root->inode_number = inode;
		root->size = size;
    }
    else{
        char * copy_path = (char *)path;
        char * dir = extract_dir(&copy_path);
		char * tdir = (char *)calloc(sizeof(char), strlen(dir));
    	strcpy(tdir,dir);
        FStree * dir_node = NULL;
        if(strlen(copy_path) == 1){     
            root->num_children++;
            if(root->children == NULL){
                root->children = (FStree **)malloc(sizeof(FStree *));
				if(strcmp(type,"directory")==0){
                	root->children[0] = init_node(path, dir, root,1);
					root->children[0]->permissions=lpermissions;
					root->children[0]->type="directory";
				}
				else{	root->num_files++;
					root->children[0] = init_node(path, dir, root,2);
					root->children[0]->permissions=lpermissions;
					root->fchildren = (FSfile **)malloc(sizeof(FSfile *));
					root->fchildren[0] = init_file(path,tdir);
					root->children[0]->type="file";
				}
				root->children[0]->group_id = groupid;
				root->children[0]->user_id = userid;
				root->children[0]->c_time = lc_time;
				root->children[0]->a_time = la_time;
				root->children[0]->m_time = lm_time;
				root->children[0]->b_time = lb_time;
				root->children[0]->inode_number = inode;
				root->children[0]->size = size;
            }
            else{
                root->children = (FStree **)realloc(root->children, sizeof(FStree *) * root->num_children);
				if(strcmp(type,"directory")==0){
                	root->children[root->num_children - 1] = init_node(path, dir, root,1);
					root->children[root->num_children - 1]->permissions = lpermissions; 
					root->children[root->num_children - 1]->type = "directory";
				}
				else{
					root->num_files++;
					root->children[root->num_children - 1] = init_node(path, dir, root,2);
					root->children[root->num_children - 1]->permissions= lpermissions; 
					root->fchildren = (FSfile **)realloc(root->fchildren, sizeof(FSfile *) * root->num_files);
					root->fchildren[root->num_files - 1] = init_file(path,tdir);
					root->children[root->num_children - 1]->type = "file";
				}
				root->children[root->num_children - 1]->group_id = groupid;
				root->children[root->num_children - 1]->user_id = userid;
				root->children[root->num_children - 1]->c_time = lc_time;
				root->children[root->num_children - 1]->a_time = la_time;
				root->children[root->num_children - 1]->m_time = lm_time;
				root->children[root->num_children - 1]->b_time = lb_time;
				root->children[root->num_children - 1]->inode_number = inode;
				root->children[root->num_children - 1]->size = size;
            }
        }
        else{
            dir_node = search_node(copy_path); 
            if(dir_node != NULL){
				if(dir_node->parent!=NULL){
					dir_node->c_time=time(NULL);
					dir_node->m_time=time(NULL);
				}
                dir_node->num_children++;
                dir_node->children = (FStree **)realloc(dir_node->children, sizeof(FStree *) * dir_node->num_children);
				if(strcmp(type,"directory")==0){
                	dir_node->children[dir_node->num_children - 1] = init_node(path, dir, dir_node,1);
					dir_node->children[dir_node->num_children - 1] ->permissions = lpermissions; //S_IFDIR | 0755
					dir_node->children[dir_node->num_children - 1]->type = "directory";
				}
				else{	dir_node->num_files++;
					dir_node->children[dir_node->num_children - 1] = init_node(path, dir, dir_node,2);
					dir_node->children[dir_node->num_children - 1] ->permissions = lpermissions; //S_IFREG | 0644
					dir_node->fchildren = (FSfile **)realloc(dir_node->fchildren, sizeof(FSfile *) * dir_node->num_files);
					dir_node->fchildren[dir_node->num_files - 1] = init_file(path,dir);
					dir_node->children[dir_node->num_children - 1]->type = "file";
				}
            }
			dir_node->children[dir_node->num_children - 1]->group_id = groupid;
			dir_node->children[dir_node->num_children - 1]->user_id = userid;
			dir_node->children[dir_node->num_children - 1]->c_time = lc_time;
			dir_node->children[dir_node->num_children - 1]->a_time = la_time;
			dir_node->children[dir_node->num_children - 1]->m_time = lm_time;
			dir_node->children[dir_node->num_children - 1]->b_time = lb_time;
			dir_node->children[dir_node->num_children - 1]->inode_number = inode;
			dir_node->children[dir_node->num_children - 1]->size = size;
        }
    }
    return;
}

FSfile * init_file(const char * path,char * name){
	printf("INIT_FILE CALLED\n");
	FSfile * new = (FSfile *)malloc(sizeof(FSfile));
    new->path = (char *)calloc(sizeof(char), strlen(path) + 1);
    new->name = (char *)calloc(sizeof(char), strlen(name) + 1);
    strcpy(new->path, (char *)path);
    strcpy(new->name, (char *)name);
	new->data = (char *)calloc(sizeof(char), 1);
	new->size=0;
	new->offset=0;
	return new;
}

void load_file(const char * path, char * data){
	printf("LOAD_FILE CALLED\n");
	FSfile * file = find_file(path);
	file->data = (char *)realloc(file->data, sizeof(char) * (strlen(data) + 1));
	strcpy(file->data, data);
	file->size = strlen(data);
	return;
}


//function to insert file into FStree
void insert_file(const char * path){
	printf("INSERT_FILE CALLED\n");
	char * copy_path = (char *)path;
    char * name = extract_dir(&copy_path);
	copy_path++;
    if(strlen(copy_path) == 0){ 
		root->num_children++;
        if(root->children == NULL){ 
            root->children = (FStree **)malloc(sizeof(FStree *));
            root->children[0] = init_node(path, name, root,0);
        }
        else{ 
            root->children = (FStree **)realloc(root->children, sizeof(FStree *) * root->num_children);
            root->children[root->num_children - 1] = init_node(path, name, root,0);
        }
		if(root->fchildren == NULL){
			root->num_files++;
			root->fchildren= (FSfile **)malloc(sizeof(FSfile *));
			root->fchildren[0]=init_file(path,name);
		}
		else{
			root->num_files++;
			root->fchildren = (FSfile **)realloc(root->fchildren, sizeof(FSfile *) * root->num_files);
			root->fchildren[root->num_files - 1]=init_file(path,name);
		}
	}
	else{
		char * rpath = reverse(reverse(copy_path,0),1);
		FStree * parent_dir_node = search_node(rpath);
		if(parent_dir_node != NULL){
		    parent_dir_node->num_children++;
		    parent_dir_node->children = (FStree **)realloc(parent_dir_node->children, sizeof(FStree *) * parent_dir_node->num_children);
		    parent_dir_node->children[parent_dir_node->num_children - 1] = init_node(path, name, parent_dir_node,0);
			if(parent_dir_node->fchildren==NULL){
				parent_dir_node->num_files++;
				parent_dir_node->fchildren= (FSfile **)malloc(sizeof(FSfile*));
				parent_dir_node->fchildren[0]=init_file(path,name);
			}
			else{
				parent_dir_node->num_files++;
				parent_dir_node->fchildren=(FSfile **)realloc(parent_dir_node->fchildren, sizeof(FSfile *) * parent_dir_node->num_files);
				parent_dir_node->fchildren[parent_dir_node->num_files -1]=init_file(path,name);
			}
        }
	}
	return;
}

void delete_file(const char *path){
	printf("DELETE_FILE CALLED\n");
	if(root == NULL){
        	return;
	}
	else{	
		int i,j;
		FStree * parent_dir_node = NULL;
		FStree * file_tree_node = search_node((char *)path);
		int file_ino;
		file_ino = file_tree_node->inode_number;
		char * typ = (char *)malloc(strlen(file_tree_node->type));
		strcpy(typ,file_tree_node->type);
		FSfile * del_file = NULL;
		char * copy_path = (char *)path;
		char * name = extract_dir(&copy_path);
	    if(strlen(copy_path) == 1){
			parent_dir_node = root;
		}
		else{
			char * rpath = reverse(reverse(copy_path,0),1);
			parent_dir_node = search_node(rpath);
		}
		parent_dir_node->c_time=time(&t);
	    parent_dir_node->m_time=time(&t);
		for(i = 0; i < parent_dir_node->num_children; i++){
			if(strcmp(parent_dir_node->children[i]->name, name) == 0){	
				for(j = i; j < parent_dir_node->num_children - 1; j++){
                    parent_dir_node->children[j] = parent_dir_node->children[j+1];
                }
                break;
            }
		}
		parent_dir_node->num_children--;
		if(parent_dir_node->num_children == 0){
            parent_dir_node->children = NULL;
        }
        else{
            parent_dir_node->children = (FStree **)realloc(parent_dir_node->children,sizeof(FStree *) * parent_dir_node->num_children);
        }
		for(i = 0; i < parent_dir_node->num_files; i++){
			if(strcmp(parent_dir_node->fchildren[i]->name, name) == 0){
                del_file = parent_dir_node->fchildren[i];
				for(j = i; j < parent_dir_node->num_files - 1; j++){
					parent_dir_node->fchildren[j] = parent_dir_node->fchildren[j+1];
				}
				break;
			}
		}
		parent_dir_node->num_files--;
		if(parent_dir_node->num_files == 0){
        	parent_dir_node->fchildren = NULL;
        }
        else{
            parent_dir_node->fchildren = (FSfile **)realloc(parent_dir_node->fchildren,sizeof(FSfile *) * parent_dir_node->num_files);
        }
		delete_metadata_block(typ,file_ino);
		update_node_wrapper(parent_dir_node, 0);
		free(del_file);
    }
	return;
}

int delete_node(const char * path){
    printf("DELETE_NODE CALLED\n");
    if(root == NULL){
        return 0;
    }
    else{
        char * copy_path = (char *)path;
        FStree * dir_node = NULL;
        int i, j;
        if(strlen(copy_path) == 1){
            printf("Cannot delete root directory!\n"); 
            return -1;
        }
        else{
            dir_node = search_node(copy_path);  
			if(dir_node->children != NULL){
				return -1;
			}
	    	dir_node->parent->c_time=time(&t);
	    	dir_node->parent->m_time=time(&t);
            if(dir_node->children != NULL){
                for(i = dir_node->num_children - 1; i >= 0; i--){
                    if(strcmp(dir_node->type, "directory") == 0){
                        delete_node((const char *)dir_node->children[i]->path);       
                    }
                }
            }
            while(dir_node->num_files > 0){
                delete_file(dir_node->fchildren[dir_node->num_files - 1]->path);
            }
            for(i = 0; i < dir_node->parent->num_children; i++){
                if(dir_node->parent->children[i] == dir_node){
                    for(j = i; j < dir_node->parent->num_children - 1; j++){
                        dir_node->parent->children[j] = dir_node->parent->children[j+1];
                    }
                    break;
                }
            }
            dir_node->parent->num_children--; 
            if(dir_node->parent->num_children == 0){
                dir_node->parent->children = NULL;
            }
            else{
                dir_node->parent->children = (FStree **)realloc(dir_node->parent->children,sizeof(FStree *) * dir_node->parent->num_children);
            }
			delete_metadata_block(dir_node->type,dir_node->inode_number);
			update_node_wrapper(dir_node->parent, 0);
			free(dir_node);
            return 0;
        }
    }
    return 0;
}

FSfile * find_file(const char * path){
	printf("FIND_FILE CALLED\n");
	char * copy_path = (char *)path;
    char * name = extract_dir(&copy_path);
	copy_path++;
	FStree * parent_dir_node;
	FSfile * my_file;
	int i;
	if(strlen(copy_path) == 0){ 
		parent_dir_node = root;
	}
	else{
		char * rpath = reverse(reverse(copy_path,0),1);
		parent_dir_node = search_node(rpath);
	}
	for(i = 0;i < parent_dir_node->num_files; i++){
		if(strcmp(parent_dir_node->fchildren[i]->name, name) == 0){
			my_file = parent_dir_node->fchildren[i];
			return my_file;
		}
	}
	return NULL;
}

void move_node(const char * from,const char * to){
	printf("MOVE_NODE CALLED\n");
	int i,j,flag = 0;
	char * copy_frompath = (char *)from;
	char * copy_topath = (char *)to;
	FStree * dir_node = search_node(copy_frompath);
	FStree * todir =  search_node(copy_topath);
	if(dir_node != NULL && todir != NULL){
		if(strcmp(todir->type,"file") == 0)
			delete_file(copy_topath);
		else
			delete_node(copy_topath);
	}
	char temp_path[20];
	if(dir_node != NULL){
		char * name = extract_dir(&copy_frompath);
		copy_frompath++;
		FStree * parent_dir_node;
		if(strlen(copy_frompath) == 0){ 
			parent_dir_node = root;
		}
		else{
			char * rpath = reverse(reverse(copy_frompath,0),1);
			parent_dir_node = search_node(rpath);
		}
		char * toname = extract_dir(&copy_topath);
		copy_topath++;
		FSfile * file_node;
		FStree * to_parent_dir_node;
		if(search_node((char *)to)==NULL)
			flag=1;
		if(strlen(copy_topath) == 0){ 
			to_parent_dir_node = root;
		}
		else{
			char * r_topath = reverse(reverse(copy_topath,0),1);
			to_parent_dir_node = search_node(r_topath);
		}
		to_parent_dir_node->num_children++;
		to_parent_dir_node->c_time=time(NULL);
		to_parent_dir_node->children = (FStree **)realloc(to_parent_dir_node->children,sizeof(FStree *) * to_parent_dir_node->num_children);
		dir_node->a_time = time(NULL);
		to_parent_dir_node->children[to_parent_dir_node->num_children - 1]=dir_node;		
		for(i = 0;i < parent_dir_node->num_children; i++){
			if(strcmp(parent_dir_node->children[i]->name,name)==0){
				for(j = i; j < parent_dir_node->num_children-1; j++){
					parent_dir_node->children[j]=parent_dir_node->children[j+1];
				}
				break;
			}
		}
		parent_dir_node->num_children--;
		if(parent_dir_node->num_children == 0){
            parent_dir_node->children = NULL;
        }
        else{
            parent_dir_node->children = (FStree **)realloc(parent_dir_node->children,sizeof(FStree *) * parent_dir_node->num_children);
        }
		
		if(strcmp(dir_node->type,"file")==0){
			file_node = find_file(from);
			to_parent_dir_node->num_files++;
			to_parent_dir_node->fchildren = (FSfile **)realloc(to_parent_dir_node->fchildren,sizeof(FSfile *) * to_parent_dir_node->num_files);
			to_parent_dir_node->fchildren[to_parent_dir_node->num_files - 1]=file_node;
			parent_dir_node->num_files--;
			if(parent_dir_node->num_files == 0){
                parent_dir_node->fchildren = NULL;
            }
            else{
                parent_dir_node->fchildren = (FSfile **)realloc(parent_dir_node->fchildren,sizeof(FSfile *) * parent_dir_node->num_files);
            }
			if(flag == 1){
				file_node->name=toname;
				file_node->path=(char *)to;
			}
		}
		strcpy(temp_path,to_parent_dir_node->path);
		dir_node->parent=to_parent_dir_node;
		if(flag == 1){
			dir_node->name=toname;
			dir_node->path=(char *)to;
		}
		if(strcmp(dir_node->type,"directory")==0){
			path_update(dir_node,temp_path);
		}
	}		
}

void path_update(FStree * dir_node,char * topath){
	printf("PATH_UPDATE CALLED\n");
	FStree * temp=dir_node;
	int i,j;
	temp->path = realloc(temp->path, strlen(topath) + strlen(temp->name) + 1);
	memset(temp->path,0,strlen(topath)+strlen(temp->name));
	strcat(temp->path,topath);
	temp->path[strlen(topath)] = '/';
	strcat(temp->path,temp->name); 
	temp->path[strlen(topath)+strlen(temp->name)+1] = '\0';
	for(i = 0;i < temp->num_children; i++){
		for(j = 0;j < temp->num_files; j++){
			memset(temp->fchildren[j]->path,0,strlen(temp->path)+strlen(temp->name));
			strcat(temp->fchildren[j]->path,temp->path);
			temp->fchildren[j]->path[strlen(temp->path)]='/';
			strcat(temp->fchildren[j]->path,temp->fchildren[j]->name);
			temp->fchildren[j]->path[strlen(temp->path)+strlen(temp->name)+1]='\0';
		}
		temp->children[i]->parent=dir_node;
		path_update(temp->children[i],temp->path);
	}
}
	

