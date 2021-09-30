#include "../include/fsoperations.h"

int do_getattr(const char *path, struct stat *st){
	printf( "GETATTR CALLED\n" );
	printf(">>>>>>>>>>>>>>>> call get attr : %s\n", path);
	char * copy_path = (char *)path;
	FStree * dir_node = NULL;
	FSfile * file_node;

	if(strlen(copy_path) > 1){
		dir_node = search_node(copy_path);
	}
	else if(strlen(copy_path) == 1){
		dir_node = root;
	}
	
	if(dir_node == NULL){
		return -ENOENT;
	}
	else{
		if (strcmp(dir_node->type, "directory") == 0){
			st->st_nlink = 2;
		}
		else{
		 	st->st_nlink = 1;
			char * temp = deserialize_file_data(dir_node->inode_number);
			if(temp!='\0'){
				load_file(path,temp);
				file_node=find_file(path);
			 	st->st_size = file_node->size;
				st->st_blocks = (((st->st_size) / 512) + 1);
			}
			else{
				st->st_size = 0;
				st->st_blocks = 0;
			}
		 }
	 }
	st->st_nlink += dir_node->num_children;
	st->st_mode = dir_node->permissions;
	st->st_uid = dir_node->user_id; 
	st->st_gid = dir_node->group_id;
	st->st_atime = dir_node->a_time;
	st->st_mtime = dir_node->m_time;
	st->st_ctime = dir_node->c_time;
	return 0;
}
	
int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
	printf("READDIR CALLED\n");
		
	filler(buffer, ".", NULL, 0 ); 
	filler(buffer, "..", NULL, 0 );
		
	char * copy_path = (char *)path;
	int i = 0;
	FStree * dir_node = NULL;

	if(strlen(copy_path) > 1){
		dir_node = search_node(copy_path);
	}
	else if(strlen(copy_path) == 1){
		dir_node = root;
	}

	if(dir_node == NULL){
		return -ENOENT;
	}
	else{
		dir_node->a_time=time(NULL);
		for(i = 0; i < dir_node->num_children; i++){
			filler( buffer, dir_node->children[i]->name, NULL, 0 );
		}
	}
	return 0;
}
	
int do_mkdir(const char * path, mode_t x){
	printf("MKDIR CALLED\n");
	insert_node(path);
	FStree * node = search_node((char *)path);
	if(node != NULL){
		serialize_metadata_wrapper(node);
		if(node->parent != NULL){
			update_node_wrapper(node->parent, 0);
		}
	}
	return 0;
}
	
int do_rmdir(const char * path){
	printf("GETATTR CALLED\n");
	int ret = delete_node(path);
	if(ret < 0){
		return -ENOTEMPTY;
	}
	return 0;
}
	
int do_mknod(const char * path, mode_t x, dev_t y){
	printf("MKNOD CALLED\n");
	insert_file(path);
	FStree * node = search_node((char *)path);
	if(node != NULL){
		serialize_metadata_wrapper(node);
		if(node->parent != NULL){
			update_node_wrapper(node->parent, 0);
		}
	}
	return 0;
}
	
int do_open(const char *path, struct fuse_file_info *fi) {
	printf("OPEN CALLED\n");
	FStree * my_file_tree_node = search_node((char *)path);
	char * temp = deserialize_file_data(my_file_tree_node->inode_number);
	if(temp != '\0'){
		load_file(path,temp);
	}
	return 0;
}
	
int do_unlink(const char * path){
	printf("UNLINK CALLED\n");
	delete_file(path);
	return 0;
}

int do_access(const char * path, int mask){
	printf("ACCESS CALLED\n");
	char * copy_path = (char *)path;
	uid_t u = getuid();
	gid_t g = getgid();
	int per_flag=0;
	FStree * my_file_tree_node;
	if(strlen(copy_path) == 1){
        my_file_tree_node = root;
	}
	else{
		my_file_tree_node = search_node((char *)path);
	}
	if(mask == 0){
		mask = 1;
	}
	mode_t p = my_file_tree_node->permissions;
	switch(mask){
		case 1:{
				if(u == my_file_tree_node->user_id){
					p = p & S_IXUSR;
					if(p == 0100)
						per_flag = 1;
				}
				else if(g == my_file_tree_node->group_id){
					p = p & S_IXGRP;
					if(p == 010)
						per_flag=1;
				}
				else{
					p = p & S_IXOTH;
					if(p == 01)
						per_flag = 1;
				}
				break;
			}
		case 4:{
				if(u == my_file_tree_node->user_id){
					p = p & S_IRUSR;
					if(p == 0400)
						per_flag = 1;
				}
				else if(g == my_file_tree_node->group_id){
					p = p & S_IRGRP;
					if(p == 040)
						per_flag = 1;
				}
				else{
					p = p & S_IROTH;
					if(p == 04)
						per_flag = 1;
				}
				break;
			}
		case 2:{
				if(u == my_file_tree_node->user_id){
					p = p & S_IWUSR;
					if(p == 0200)
						per_flag = 1;
				}
				else if(g == my_file_tree_node->group_id){
					p = p & S_IWGRP;
					if(p == 020)
						per_flag = 1;
				}
				else{
					p = p & S_IWOTH;
					if(p == 02)
						per_flag = 1;
				}
				break;			
			}
		}
		if(per_flag == 1)
			return 0;

		return -EACCES;
}
	
int do_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi) {
	printf("READ CALLED\n");

	if(do_access(path,R_OK)!=0){
		return -EACCES;
	}

	size_t len;
	FStree * my_file_tree_node;
	FSfile * my_file;
	my_file_tree_node = search_node((char *)path);
	my_file = find_file(path);
	char * temp = deserialize_file_data(my_file_tree_node->inode_number);

	if(temp != '\0')
		load_file(path,temp);

	if(my_file_tree_node != NULL){	
		my_file_tree_node->a_time = time(NULL);
		len = strlen(my_file->data);
		if(len == 0){
			return 0;
		}
		memcpy(buf, my_file->data + offset, size);
		return size;
	}
	return -ENOENT;
}
	
int do_chmod(const char *path, mode_t new){	
	printf("CHMOD CALLED\n");
	FStree * current;
	current = search_node((char *)path);
	if(current != NULL){
		current->c_time=time(NULL);
		current->permissions = new;
		update_node_wrapper(current, 1);
		return 0;
	}
	return -ENOENT;
}

int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf("WRITE CALLED and path is :%s\n",path);
	if(do_access(path,W_OK)!=0){
		return -EACCES;
	}

	FStree * my_file_tree_node;
	FSfile * my_file;
	my_file = find_file(path);

	if(my_file != NULL){
		my_file_tree_node = search_node((char *)path);
		my_file_tree_node->path = (char *)realloc(my_file_tree_node->path, sizeof(char) * strlen(path)+1);
		strcpy(my_file_tree_node->path,path);
		my_file_tree_node->path[strlen(my_file_tree_node->path)]='\0';
		my_file_tree_node->m_time = time(NULL);
		my_file_tree_node->c_time = time(NULL);
		my_file->data = (char *)realloc(my_file->data, sizeof(char) * (size + offset + 1));
		my_file->size = size + offset;
		memset((my_file->data) + offset, 0, size);
		memcpy((my_file->data) + offset, buf, size);
		my_file_tree_node->size = size + offset;
		if(offset>0){
			my_file->data[strlen(buf)+offset-1]='\0';	
		}
		else{
			my_file->data[strlen(buf)]='\0';
		}
		int i, j=0, k, flag =0;
		char sub [] = ".goutputstream";
		for(i=0; path[i]; i++)
		{
			if(path[i] == sub[j])
			{
				for(k=i, j=0; path[k] && sub[j]; j++, k++)
					if(path[k]!=sub[j])
						break;
				if(!sub[j]){
					flag = 1;
        			}
			}
		}
		if(flag == 0){
			serialize_filedata_wrapper(my_file_tree_node->inode_number,my_file->data,my_file_tree_node);
		}
		memset((char *)buf, 0, strlen(buf));
		return size;
	}
	return -ENOENT;
}

int do_utimens(const char *path, struct utimbuf *tv){
	printf("UTIMENS CALLED\n");
	return 0;
}

int do_rename(const char* from, const char* to){
	printf("RENAME CALLED\n");
	FStree * src;
	FStree * dst;
	src = search_node((char *)from);
	if(src == NULL){
		return -ENOENT;
	}
	dst = search_node((char *)to);
	if(dst != NULL){
		if(strcmp(src->type,"directory") == 0 && strcmp(dst->type, "file") == 0){
			return -EPERM;
		}
	}
	move_node(from,to);
	if(strcmp(dst->type,"file")==0){
		int i, j=0, k, flag =0;
		char sub [] = "goutputstream";
		for(i=0; from[i]; i++)
		{
			if(from[i] == sub[j])
			{
				for(k=i, j=0; from[k] && sub[j]; j++, k++)
					if(from[k]!=sub[j])
						break;
				if(!sub[j]){
					flag = 1;
				}
			}
		}
		if(flag == 0){
			delete_metadata_block(src->type,src->inode_number);
			update_node_wrapper(src->parent, 0);
		}
	
		dst = search_node((char *)to);
		serialize_metadata_wrapper(dst);
		if(dst->parent != NULL){
			update_node_wrapper(dst->parent, 0);
		}
		FSfile * my_file = find_file((char*)to);
		serialize_filedata_wrapper(dst->inode_number,my_file->data,dst);
	}
	return 0;
}

int do_truncate(const char *path, off_t size){
	printf("TRUNCATE CALLED\n");
	FSfile * my_file;
	my_file = find_file(path);
	FStree * my_file_tree_node;
	my_file_tree_node = search_node((char *)path);
	if(my_file != NULL){
		if(size <= 0){
			free(my_file->data);
			my_file->data = (char *)calloc(1,sizeof(char));
			my_file->size = 0;
			load_file(path,my_file->data);
			serialize_filedata_wrapper(my_file_tree_node->inode_number,my_file->data,my_file_tree_node);
		}
		else{
			char *buf;
			buf = (char *)malloc(sizeof(char)*(size + 1));
			strncpy(buf, my_file->data, size);
			free(my_file->data);
			my_file->data = (char *)calloc(size + 1, sizeof(char));
			my_file->data[strlen(my_file->data)-1]='\0';
			strcpy(my_file->data,buf);
			my_file->size=size;
			load_file(path,my_file->data);
			serialize_filedata_wrapper(my_file_tree_node->inode_number,my_file->data,my_file_tree_node);
	
		}
		return 0;
	}
	return -ENOENT;
}
