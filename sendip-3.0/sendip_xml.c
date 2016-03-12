/* sendip_xml.c - prase an XML file to extract sendip arguments
 * ChangeLog since 3.0 release:
 */

#include <stdio.h>
#include <expat.h>
#include <string.h>
#include "sendip_module.h"

#define ALLOC_SIZE 10

typedef struct {
	int argc;
	char ***argv;
	bool parse_error;
	const char *in_thing;
	int alloced;
} xml_userdata;

static void add_arg(xml_userdata *data, const char *arg, const char *val) {
	if(data->alloced==-1) {
		*(data->argv) = malloc(ALLOC_SIZE*sizeof(char *));
		if(*(data->argv) == NULL) {
			fprintf(stderr,"OUT OF MEMORY");
			exit(1);
		}
		data->alloced = ALLOC_SIZE;
	}
	if(data->argc+(val==NULL)?1:2 > data->alloced) {
		data->alloced+=ALLOC_SIZE;
		*(data->argv) = realloc(*(data->argv),data->alloced*sizeof(char *));
		if(*(data->argv) == NULL) {
			fprintf(stderr,"OUT OF MEMORY");
			exit(1);
		}
	}
	(*data->argv)[data->argc++]=strdup(arg);
	if(val)
		(*data->argv)[data->argc++]=strdup(val);

}

static void startElement(void *userData, const char *name, const char **atts) {
	const char **at;
	xml_userdata *data = (xml_userdata *)userData;

	printf("START ELEMENT : \n");
	printf(" NAME %s\n",name);
	printf(" IN_THING %s\n",data->in_thing);

	if(!strcmp(name,"header")) {
		printf("  HEADER\n");
		for(at=atts;*at != NULL;at++) {
			if(strcmp(*at,"type")) {
				data->parse_error = TRUE;
				fprintf(stderr,"Unrecognized attribute %s of element header\n",*at);
			} else {
				add_arg(data,"-p",*(++at));
				data->in_thing=strdup(*at);
			}
		}
	} else if(!strcmp(name,"option")) {
		printf("  OPTION\n");
		const char *opt=NULL;
		const char *val=NULL;
		for(at=atts;*at != NULL;at++) {
			if(!strcmp(*at,"name")) {
				opt=*(++at);
				//opt = malloc(strlen(*at)+1);
				//sprintf((char *)opt,"-%s",*at);
				printf("  NAME: %s\n",opt);
			} else if(!strcmp(*at,"value")) {
				val=*(++at);
				printf("  VAL: %s\n",val);
			} else {
				fprintf(stderr,"Unrecognized attribute %s of element option\n",*at);
				data->parse_error=TRUE;
			}
		}
		if(opt) {
			char *combo=malloc(strlen(data->in_thing)+strlen(opt)+val?strlen(val):1+2);
			sprintf(combo,"%s=%s=%s",data->in_thing,opt,val?val:".");
			printf("  COMBO %s\n",combo);
			add_arg(data,"-X",combo);
		} else {
			fprintf(stderr,"Option element with no name attribute found\n");
		}
	} else if(!strcmp(name,"data")) {
		data->in_thing = "data";
	} else if(strcmp(name,"packet")) {
		fprintf(stderr,"Unrecognized element %s found\n",name);
		data->parse_error=TRUE;
	}
}

static void endElement(void *userData, const char *name) {
	xml_userdata *data = (xml_userdata *)userData;
	if(data->in_thing) {
		if(!strcmp(name,"header") || !strcmp(name,"data")) {
			free((void *)data->in_thing);
			data->in_thing = NULL;
		}
	}
}

static void doData(void *userData, const char *s, int len) {
	xml_userdata *data = (xml_userdata *)userData;
	if(data->in_thing && !strcmp(data->in_thing,"data")) {
		char *d = malloc(len);
		memcpy(d,s,len);
		add_arg(data,"-d",d);
		free(d);
	}
}

int parse_xml(const char *xmlfile, char ***new_argv) {
	xml_userdata mydata;

	char buf[BUFSIZ];
	bool done;

	FILE *infile = fopen(xmlfile,"rt");
	if(infile==NULL) {
		fprintf(stderr,"Could not open file %s\n",xmlfile);
		return 0;
	}

	mydata.argc=0;
	mydata.argv=new_argv;
	mydata.parse_error=FALSE;
	mydata.alloced=-1;
	mydata.in_thing=NULL;

	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser,&mydata);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, doData);
	do {
		size_t len = fread(buf, 1, sizeof(buf), infile);
		done = len < sizeof(buf);
		if((XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) || (mydata.parse_error)) {
			fprintf(stderr,"%s at line %d reading %s\n",
					  XML_ErrorString(XML_GetErrorCode(parser)),
					  XML_GetCurrentLineNumber(parser),
					  xmlfile);
			if(*mydata.argv) {
				int i;
				for(i = 0; i<mydata.argc;i++)
					free(*(mydata.argv)[i]);
				free(*(mydata.argv));
			}
			*mydata.argv=NULL;
			mydata.argc=0;
			done=TRUE;
		}
	} while(!done);

	fclose(infile);

	return mydata.argc;
}
