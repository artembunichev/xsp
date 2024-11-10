/*xsp-x selection paste. retrieves clipboard selection and spews it to stdout.
build: cc xsp xsp.c -lX11*/
#include<X11/Xlib.h>
#include<unistd.h>
#include<stdio.h>
static Atom CLP;//CLIPBOARD standard atom
static Atom UTF;//type we want to convert selection to
static Atom SEL;//arbitrary atom for our own window to store selection data in

/*creates dummy window,makes a request for providing a clipboard x selection in ut8 format and
asks to put the result in its arbitrary property can be accessed with SEL atom. if the selection data was
converted and delievered successfully it redirects it to stdout*/
int
main()
{
Display* dpl=XOpenDisplay(NULL);
if(dpl==NULL){dprintf(2,"can't open display.\n");return 0;}
Window w=XCreateSimpleWindow(dpl,DefaultRootWindow(dpl),0,0,1,1,0,0,0);
CLP=XInternAtom(dpl,"CLIPBOARD",False);
UTF=XInternAtom(dpl,"UTF8_STRING",False);
SEL=XInternAtom(dpl,"SEL",False);
//request selection from window owns it right now in UTF format
XConvertSelection(dpl,CLP,UTF,SEL,w,CurrentTime);
XEvent ev;
/*dummy values exist only to satisfy XGetWindowProperty function interface.
they do not carry any valuable information*/
int di;Atom tp,da;
unsigned long sz,dul;//selection data size(length),
unsigned char *sel=NULL;
/*response to our clipboard selection request wouldn't be synchronous
so we need to wait for it by the means of infinite loop*/
while(1)
{
	XNextEvent(dpl,&ev);
	switch(ev.type){
/*event indicates that we have got a response to selection request we've done*/
	case SelectionNotify:{
	XSelectionEvent* snev=&(ev.xselection);
	/*it basically means that selection owner isn't capable of converting
	selection data to utf8 format hence our property we asked him to put the
	resulting data in is empty*/
	if(snev->property==None){
	dprintf(2,"can't convert selection.\n");return 1;
	}
	else//selection ownew has successfully put clipboard selection data into our windows property
	{
	/*make this request in order to obtain data size*/
	XGetWindowProperty(dpl,w,SEL,0,0,False,AnyPropertyType,&tp,&di,&dul,
&sz, &sel);
	/*use data size has just been figured out to finally get selection data into sel variable*/
	XGetWindowProperty(dpl,w,SEL,0,sz,False,AnyPropertyType,&da,
&di,&dul,&dul,&sel);
	write(1,sel,sz);//write data to stdout
	return 0;//exit the program
	}
	}
	}
};
};
