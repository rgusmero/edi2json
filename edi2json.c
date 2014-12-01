/* @(#) $Id: edi2json.c 10 2014-12-01 07:30:00 $ */

/*
 * Copyright (c) 2014 Riccardo Gusmeroli.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the author(s) of this software may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * AUTHORS OF THIS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <libedi.h>
#include <json.h>

int main(int argc, char **argv) {

	char *msg, *m;
	FILE *f;
	int size, read_size;
	int is, ie, iv;
	edi_params_t params;
	edi_parser_t *parser;
	edi_interchange_t *edi;
	edi_segment_t *s;
	edi_element_t *el;
	JsonNode *root, *s_json, *el_json;

	if (argc!=3) {
		
		printf("USAGE: edi2json <edifile> <jsonfile>\n");
		return 1;
	}
	
	// OPEN FILE FOR READING
	f=fopen(argv[1],"rb");
	if (!f) {
	
		printf("ERROR OPENING FILE %s\n",argv[1]);
		return 2;
	}
	
	// DETERMINE FILE SIZE
	fseek(f,0,SEEK_END);
	size=ftell(f);
	fseek(f,0,SEEK_SET);

	msg=(char *)malloc(size*sizeof(char));
	if (!msg) {
	
		printf("NOT ENOUGH MEORY (SIZE=%d)\n",size*sizeof(char));
		fclose(f);
		return 3;
	}
	
	read_size=fread(msg,sizeof(char),size,f);
	if (read_size!=size*sizeof(char)) {
	
		printf("ERROR READING INTO DUFFER (%d!=%d)\n",read_size,size*sizeof(char));
		free(msg);
		fclose(f);
		return 3;
		
	} else {
	
		fclose(f);
	}
	
	printf("- PARSING FILE \"%s\"...\n",argv[1]);
	
	m=msg;
	if (strncmp(m,"UNA",3)!=0) {
	
		// No UNA gibe, use default
		params.version=-1; 
		
	} else {
	
		// Skip U, N, A
		m+=3;
		params.version=EDI_VERSION;
		params.subelement_separator=*m++; // default :
		params.element_separator=*m++; // default +
		m++; // Decimal character
		params.escape=*m++; // default ?
		m++; // Reserved
		params.segment_separator=*m++; // default '
		
		params.tag_separator=params.element_separator;
	}
	
	parser=edi_parser_create(params.version<0?NULL:&params);
	if (!parser) {
	
		printf("ERROR CREATING PARSER\n");
		free(msg);
		return 3;
	}
	
	edi=edi_parser_parse(parser,m);
	if (!edi) {
	
		printf("FATAL ERROR PARSING FILE\n");
		edi_parser_destroy(parser);
		free(msg);
		return 4;
	}
	switch (edi_parser_error(parser)) {
	
		case EDI_ERR_NONE:	/* No error occurred */
			break;
		case EDI_ERR_SYSTEM:     /* See errno for further information */
			printf("SYSTEM ERROR WHILE PARSING (%d)",errno);
			edi_interchange_destroy(edi);
			edi_parser_destroy(parser);
			free(msg);
			fclose(f);
			return 5;
		
		case EDI_ERR_UNTERMINATED:    /* Parsing ended before the segment was terminated */
			printf("WARNING: UNTERMINATED MESSAGE\n");
			break;
		
		case EDI_ERR_EMPTY:      /* Parsing ended because the message was empty */			
			break;
			
		default:
			printf("UNKNOWN PARSING ERROR\n");
			edi_interchange_destroy(edi);
			edi_parser_destroy(parser);
			free(msg);
			fclose(f);
			return 6;
	}
	
	printf("- PARSED %d SEGMENTS...\n",edi->nsegments);
	free(msg);
	
	printf("- CONVERTING TO JSON...\n");	
	
	root=json_mkarray();
	for(is=0; is<edi->nsegments; is++) { // Foreach segment
	
		s=edi->segments+is;
		s_json=json_mkarray();
		for(ie=0; ie<s->nelements; ie++) { // Foreach element in seg.
		
			el=s->elements+ie;
			el_json=json_mkarray();
			
			if(el->type == EDI_ELEMENT_SIMPLE) {
			
				json_append_element(el_json,json_mkstring(el->simple.value));				
				
			} else {
							
				for(iv= 0; iv<el->composite.nvalues; iv++) { // For each value in element
				
					json_append_element(el_json,json_mkstring(el->composite.values[iv]));		
					
				}
			}
			
			json_append_element(s_json,el_json);
		}
		
		json_append_element(root,s_json);
	}

	edi_interchange_destroy(edi);
	edi_parser_destroy(parser);
	
	msg=json_stringify(root,"\t");
	json_delete(root);
	
	f=fopen(argv[2],"w+t");
	if (!f) {
	
		printf("ERROR OPENING FILE \"%s\" FOR WIRING\n",argv[2]);
		
	} else {
	
		printf("- SENDING JSON TO FILE \"%s\"...\n",argv[2]);
		fwrite(msg,strlen(msg),sizeof(char),f);
		fclose(f);
	}
	free(msg);
 	
	printf("DONE.\n");
	return 0;
}
