#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <glib.h>

int main( int argc, char *argv[] ) {
    // general storage needs
    GArray *results;
    GString *tmp = g_string_new("");

    // libpq
    PGconn *connection = NULL;
    PGresult *result;

    // connection-settings
    const char *keywords[] = {"dbname", "hostaddr", "user", "password", "port", NULL};
    const char *values[] = {"Transaktionssystem-1", "136.172.14.14", "postgres", "postgres", "48142", NULL};
    
    // open a connection to the database
    connection = PQconnectdbParams(keywords, values, 0);
    
    // check if the connection has been successfully established
    if (PQstatus(connection) != CONNECTION_OK) {
            // ERROR: print the error message
            fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(connection));
    }
    else {
        tmp = g_string_new("SELECT * FROM jtest1;");
        // query the database
        result = PQexec(connection, tmp->str);

        if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            // ERROR: print the error message
            fprintf(stderr, "Select1 failed: %s", PQresultErrorMessage(result));
            // empty the result to prevent leakage
            PQclear(result);
        }
        else {
            int i = 0;
            int j = 0;

            printf("jtest1:\n");
            // go through the table names and print them
            for (i = 0; i < PQnfields(result); i++) {
                printf("%-15s", PQfname(result, i));
            }
            printf("\n\n");

            // go through all result and save them into a two-dimensional GArray
            for (i = 0; i < PQntuples(result); i++) {
                for (j = 0; j < PQnfields(result); j++) {
                    // read the result and print it
                    printf("%-15s", PQgetvalue(result, i, j));
                }
                printf("\n");
            }

            // empty the result to prevent leakage
            PQclear(result);
        }

        tmp = g_string_new("SELECT * FROM jtest2;");
        // query the database
        result = PQexec(connection, tmp->str);

        if (PQresultStatus(result) != PGRES_TUPLES_OK)
        {
            // ERROR: print the error message
            fprintf(stderr, "Select2 failed: %s", PQresultErrorMessage(result));
            // empty the result to prevent leakage
            PQclear(result);
        }
        else {
            int i = 0;
            int j = 0;

            printf("\njtest2:\n");
            // go through the table names and print them
            for (i = 0; i < PQnfields(result); i++) {
                printf("%-15s", PQfname(result, i));
            }
            printf("\n\n");

            // go through all result and save them into a two-dimensional GArray
            for (i = 0; i < PQntuples(result); i++) {
                for (j = 0; j < PQnfields(result); j++) {
                    // read the result and print it
                    printf("%-15s", PQgetvalue(result, i, j));
                }
                printf("\n");
            }

            // empty the result to prevent leakage
            PQclear(result);
        }

        // aus den beiden Tables jtest1 und jtest2 joinen und einen einzelnen Table namens jtest3 erstellen
        tmp = g_string_new("SELECT jtest1.id, jtest1.name, jtest2.alter INTO jtest3 FROM jtest1, jtest2 WHERE jtest1.id = jtest2.id;");
        // query the database
        result = PQexec(connection, tmp->str);

        if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            // ERROR: print the error message
            fprintf(stderr, "Join failed: %s", PQresultErrorMessage(result));
            // empty the result to prevent leakage
            PQclear(result);
        }
        else {
            PQclear(result);

            // alles aus jtest3 selecten und ausgeben
            tmp = g_string_new("SELECT * FROM jtest3;");
            // query the database
            result = PQexec(connection, tmp->str);
            
            if (PQresultStatus(result) != PGRES_TUPLES_OK)
            {
                // ERROR: print the error message
                fprintf(stderr, "Select3 failed: %s", PQresultErrorMessage(result));
                // empty the result to prevent leakage
                PQclear(result);
            }
            else {            

                int i = 0;
                int j = 0;

                printf("\nJoin aus jtest1 und jtest2:\n");
                // go through the table names and print them
                for (i = 0; i < PQnfields(result); i++) {
                    printf("%-15s", PQfname(result, i));
                }
                printf("\n\n");

                // go through all result and save them into a two-dimensional GArray
                for (i = 0; i < PQntuples(result); i++) {
                    for (j = 0; j < PQnfields(result); j++) {
                        // read the result and print it
                        printf("%-15s", PQgetvalue(result, i, j));
                    }
                    printf("\n");


                }
                PQclear(result);

                // jtest3 wieder löschen
                tmp = g_string_new("DROP TABLE jtest3;");
                // query the database
                result = PQexec(connection, tmp->str);

                if (PQresultStatus(result) != PGRES_COMMAND_OK)
                {
                    // ERROR: print the error message
                    fprintf(stderr, "Drop failed: %s", PQresultErrorMessage(result));
                    // empty the result to prevent leakage
                    PQclear(result);
                }

            }        
        }
    }
}